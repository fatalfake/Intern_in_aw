#!/usr/bin/python
#coding:utf-8

import os
import sys

reload(sys)
sys.setdefaultencoding('utf8')
import argparse
import configparser
import logging
import json
import re
import rosnode
import rosparam
import rospy
import subprocess
import time
import yaml

from rosparam import upload_params
from rosparam import list_params
from rosparam import delete_param
from xml.dom.minidom import parse
import xml.dom.minidom
from yaml import load

from simlib.aw_roscore import AWRoscore
from simlib.aw_process import AWProcess

from simlib.simu_config import AW_Config
from simlib.shell_parser import parse_shell
from simlib.tookit import ColorStrBuilder, read_yaml
from simlib.envcfg import detect_pkg, alter_packpath
from simlib.runtime_manager import RuntimeManager
from simlib.test_report import HTML_REPORT

from simlib.test_record import EvaluationIndication
from simlib.test_record import RegressionCase
from simlib.test_record import RegressionInfo
from simlib.test_record import EvaluationRecord
from simlib.test_record import RegressionDBManager

class RegressionConfig(configparser.ConfigParser):
    def __init__(self):
        self.conf_file = "config/regression_env"
        super(RegressionConfig,self).__init__()
        self.read(self.conf_file, encoding="utf-8")

    def write_conf(self, section, option, value):
        if section not in self.sections():
            self.add_section(section)
        self.set(section, option, value)
        with open(self.conf_file, "w+") as f:
            self.write(f)




class TestResult(object):

    def get_version(self, vehicle):
        """
        return the current version of ros-kinetic-autowise-autowise
        """
        p = subprocess.Popen("dpkg -l|grep ros-kinetic-autowise-autowise|awk '{print $3}'", stdout=subprocess.PIPE, shell=True, env=os.environ)
        version = p.stdout.read()
        print(version)
        os.system("dpkg -l |grep autowise")
        versions = version.split("\n")

        if len(versions) > 0:
            autowise_version =  versions[0]
            if vehicle is not None:
                if autowise_version != "":
                    autowise_version = autowise_version + "_" + vehicle
                else:
                    autowise_version = vehicle
        else:
            autowise_version = ""
            if vehicle is not None:
                autowise_version = vehicle
        return autowise_version



    def save_evaluation_result(self, test_version, evaluation_result):
        """
        保存测试记录到数据库
        """
        #if evaluation_result == {} or evaluation_result is None:
        #    return
        dbmanager = RegressionDBManager()
        regression = RegressionInfo(version=test_version)
        regression.evaluationresult = []
        for casename in evaluation_result:
            caseid = None
            testcase = RegressionCase.get(dbmanager.session, casename)
            if testcase is None:
                testcase = RegressionCase(casename=casename)
                caseid = RegressionCase.add(dbmanager.session, testcase)
            else:
                caseid = testcase.id
            if caseid is None:
                return


            for key in evaluation_result[casename]:
                indicationid = None
                value = evaluation_result[casename][key]
                indication = EvaluationIndication.get(dbmanager.session, key)
                if indication is None:
                    indication = EvaluationIndication(name=key)
                    indicationid = EvaluationIndication.add(dbmanager.session, indication)
                else:
                    indicationid = indication.id
                evaluationrecord = EvaluationRecord(caseid=caseid, indicationid=indicationid, indicationvalue=value)
                regression.evaluationresult.append(evaluationrecord)
        RegressionInfo.add(dbmanager.session, regression)

        dbmanager.session.close()



class CaseRunner(object):
    def __init__(self, case_dir, record_dir,
                    stdout=sys.stdout,
                    stderr=sys.stderr,
                    port="11311",
                    vehicle=None,
                    record=False):
        self.ros_port = port
        self.record_dir = record_dir
        self.case_dir = case_dir
        self.vehicle = vehicle
        self.record = record
        self.stdout = stdout
        self.stderr = stderr
        self.param_file = "planning.yaml"
        #### evaluation
        self.evaluation_param_name = "/aw/evaluation/conf_path"
        self.evaluation_dir = "evaluation"
        self.evaluation_conf_file = "conf.yaml"
        self.evaluation_result_file = "result.txt"


    def __clean_paramserver(self):
        params = list_params("/")
        for param in params:
            delete_param(param, True)


    def __init_nodeyaml(self):
        """
        update obstacle path  to planning.yaml
        set evaluation param
        return
            param info
        """
        yamlfile = os.path.join(self.case_dir, self.param_file)
        if not os.path.exists(yamlfile):
            print("%s not found , should regenerate the case"%yamlfile)
            sys.exit(1)
        data = read_yaml(yamlfile)
        try:
            evaluation_conf_dir = os.path.join(self.case_dir, self.evaluation_dir)
            if not os.path.exists(evaluation_conf_dir):
                os.mkdir(evaluation_conf_dir)
            evaluation_conf_yaml_file = os.path.join(evaluation_conf_dir, self.evaluation_conf_file)
            if not os.path.exists(evaluation_conf_yaml_file):
                os.system("cp config/evaluation/conf.yaml %s"%evaluation_conf_dir)
            data[self.evaluation_param_name] = os.path.abspath(evaluation_conf_dir)

            for k in data['obstacle_restorer_node']:
                s = data['obstacle_restorer_node'][k]
                p = os.path.basename(s)
                c = os.path.abspath(self.case_dir)
                data['obstacle_restorer_node'][k] = os.path.join(c, p)
        except KeyError as err:
            rospy.logerr(
                err.message + " missed in the 'planning.yaml' of that case.")
            raise err
        with open(yamlfile, 'w') as f:
            yaml.dump(data, f)
        return data




    def __loadparam_main(self):
        os.environ["ROS_MASTER_URI"] = "http://127.0.0.1:" + self.ros_port
        os.environ["ROS_IP"] = "127.0.0.1"
        self.__clean_paramserver()
        params = self.__init_nodeyaml()
        try:
            if "vehicle_id" in params["aw_simulation_vehicle"]:
                vehicle_id = params["aw_simulation_vehicle"]["vehicle_id"]
                print("vhicle id: %s " % vehicle_id)
            else:
                vehicle_id = params["vehicle_pose_collector"]["vehicle_id"]
            ###制定了仿真的车辆
            if self.vehicle is not None:
                vehicle_id = self.vehicle
            if "park_id" in params["aw_simulation_vehicle"]:
                park_id = params["aw_simulation_vehicle"]["park_id"]
                print("park id: %s " % park_id)
            else:
                park_id = params["aw"]["planning"]["aw_planning"]["itinerary"]["park_id"]
            aw_config = AW_Config()
            setup_filename = aw_config.GenSetup(vehicle_id, park_id, None)
            parse_shell(os.path.join(
                os.environ["HOME"], ".autowise", "simu", setup_filename))
        except KeyError as err:
            rospy.logerr(
                err.message + " Something missed in the planning.yaml; Should create the test case again.")
            sys.exit(-1)
        os.environ["ROS_MASTER_URI"] = "http://127.0.0.1:" + self.ros_port
        os.environ["ROS_IP"] = "127.0.0.1"
        node_yaml_file = os.path.join(self.case_dir, self.param_file)
        with open(node_yaml_file) as f:
            node = load(f)
        upload_params("/", node)

        with open(self.case_dir+'/goal.yaml', 'r') as f:
            yamlfile = load(f)
        upload_params('/', yamlfile)
        return True


    def run(self):
        """
        run specific case
        specify the stdout and stderr
        privides running conf
        """
        if not os.path.exists(self.case_dir):
            logging.error("%s not found in cases"% self.case_dir)
            return
        else:
            logging.info(" %s Begin to run %s %s"% ("=" * 10, self.case_dir, "=" * 10))

        # load environment and param
        rosmap_cmd = "roslaunch  aw_hdmap hdmap_runtime_env.launch"
        rosevaluation_cmd = "roslaunch aw_evaluation start_evaluation.launch"
        rossim_cmd = "roslaunch launch/run_case_simulation_by_env.launch"
        casename = os.path.basename(self.case_dir)
        rosrecord_cmd = "rosbag record /aw/planning_info -o %s/%s"%(self.record_dir, casename)
        self.__loadparam_main()
        env = os.environ
        rosmap = AWProcess(rosmap_cmd, None, self.stdout, self.stderr, env)
        rossim = AWProcess(rossim_cmd, None, self.stdout, self.stderr, env)
        rosevaluation = AWProcess(rosevaluation_cmd, None, self.stdout, self.stderr, env)
        rosrecord = AWProcess(rosrecord_cmd, None, self.stdout, self.stderr, env)
        rosmap.run()
        rospy.sleep(3)
        ## launch evaluation ahead in case something missed at beginning
        rosevaluation.run()
        rospy.sleep(3)
        rossim.run()
        if self.record:
            rosrecord.run()

        ctrl = RuntimeManager(autopause=False, outputpath=None, dtask=True,
                              autoexit=True, casename=self.case_dir, timeout=None,runid=None, enable_keyboard=False)
        try:
            ctrl.initialize(rossim)
            ctrl.spin_til_exit()
        except KeyboardInterrupt:
            rospy.loginfo(ColorStrBuilder.notice("Keyboradinterrupt caught in main."))
        else:
            rospy.loginfo("Normal Exit.")
            rospy.sleep(3)
        finally:
            print(ColorStrBuilder.notice("Cleaning up..."))
            rossim.terminate()
            rosmap.terminate()
            rosevaluation.terminate()
            if self.record:
                rosrecord.terminate()
            rosnode.kill_nodes(['/hdmap_node','/hdmap','/evaluation_node'])

    def analyse_evaluation_result(self, evaluation_result):
        """
        根据evaluation的结果,判断是否成功
        规则1: 如果没有到达目的地,失败
        规则2: 如果发生碰撞,失败
        """
        result = True
        if "arrived" in evaluation_result and evaluation_result['arrived']  == 'success':
            result = True
        else:
            result = False
        if "collided" in evaluation_result and evaluation_result['collided'] == 'False':
            result = True
        else:
            result = False
        return result

    def get_evaluation_result(self):
        """
        解析每个case的evaluation结果
        返回dict
        如果结果文件缺失返回{}
        """
        result_file = os.path.join(self.case_dir, self.evaluation_dir,
                                self.evaluation_result_file)
        if not os.path.exists(result_file):
            return {}
        evaluation_result = {}
        module_pattern = r"\[Module: (.*?)\]"
        arrival_check_pattern=r"\[Module: ArrivalCheck\] (.*?),"
        arrival_success_time_pattern=r"\[Module: ArrivalCheck\] Arrival time: (.*?)seconds."
        arrival_timeout_time_pattern=r"\[Module: ArrivalCheck\] Arrival time: Timeout.\(near (.*?)seconds."

        collided_pattern = r"\[Module: CollisionCheck\] collided: (.*)"
        min_dist_pattern = r"\[Module: CollisionCheck\] min_dist: (.*?)cm"
        quantile_pattern = r"\[Module: CollisionCheck\]\[Main\] (.*?)% quantile: \[(.*?), (.*?)\)cm. (.*?)% quantile: \[(.*?), (.*?)\)cm"

        comfort_linear_pattern = r"\[Module: ComfortCheck\]\[Linear\] Max_Pos_Acc: (.*?) m/s\^2, Max_Neg_Acc: (.*?) m/s\^2, Max_Pos_Jerk: (.*?) m/s\^3, Max_Neg_Jerk: (.*?) m/s\^3. Sudden_Acc: (.*?) times, Sudden_Deacc: (.*?) times."
        comfort_angular_pattern = r"\[Module: ComfortCheck\]\[Angular\] Max_Pos_Omg: (.*?) rad/s, Max_Neg_Omg: (.*?) rad/s, Max_Pos_Alpa: (.*?) rad/s\^2, Max_Neg_Alpa: (.*?) rad/s\^2. Rapid_Turn_Acc: (.*?) times, Rapid_Turn_Deacc: (.*?) times."

        trajectory_lateral_pattern = r"\[Module: TrajectoryCheck\]\[lateral diff\]\[window_size: 1 s\] Max: (.*?) cm. Mediam: (.*?) cm"
        trajectory_angle_pattern =   r"\[Module: TrajectoryCheck\]\[angle   diff\]\[window_size: 1 s\] Max: (.*?) degree. Medium: (.*?) degree."
        with open(result_file, "r") as f:
            for line in f:
                module_search = re.findall(module_pattern, line, re.M|re.I)
                if len(module_search) > 0:
                    module_name = module_search[0]
                    if module_name == "ArrivalCheck":
                        arrived_search = re.findall(arrival_check_pattern, line)
                        if len(arrived_search) > 0:
                            evaluation_arrived = arrived_search[0]
                            evaluation_result["arrived"] = evaluation_arrived
                            continue
                        arrival_timeout_search = re.findall(arrival_timeout_time_pattern, line)
                        if len(arrival_timeout_search)> 0:
                            evaluation_result["timeout"] = True
                            evaluation_result["run_time"] = arrival_timeout_search[0]
                            continue
                        arrival_success_time_search = re.findall(arrival_success_time_pattern, line)
                        if len(arrival_success_time_search) > 0:
                            evaluation_result["timeout"] = False
                            evaluation_result["run_time"] = arrival_success_time_search[0]
                            continue
                    elif module_name == "CollisionCheck":
                        collied_search = re.findall(collided_pattern, line)
                        if len(collied_search) > 0:
                            evaluation_result["collided"] = collied_search[0]
                            continue
                        min_dist_search = re.findall(min_dist_pattern, line)
                        if len(min_dist_search) > 0:
                            evaluation_result["min_dist"] = min_dist_search[0]
                            continue
                        quantile_search = re.findall(quantile_pattern, line)
                        if len(quantile_search) > 0 and len(quantile_search[0]) > 5:
                            evaluation_result["quantile_5"] = quantile_search[0][1]
                            evaluation_result["quantile_10"] = quantile_search[0][4]
                            continue
                    elif module_name == "ComfortCheck":
                        linear_search = re.findall(comfort_linear_pattern, line)
                        if len(linear_search) > 0 and len(linear_search[0]) == 6:
                            evaluation_result["max_pos_acc"] = linear_search[0][0]
                            evaluation_result["max_neg_acc"] = linear_search[0][1]
                            evaluation_result["max_pos_jerk"] = linear_search[0][2]
                            evaluation_result["max_neg_jerk"] = linear_search[0][3]
                            evaluation_result["sudden_acc"] = linear_search[0][4]
                            evaluation_result["sudden_deacc"] = linear_search[0][5]
                            continue
                        angular_search = re.findall(comfort_angular_pattern, line)
                        if len(angular_search) > 0 and len(angular_search[0]) == 6:
                            evaluation_result["max_pos_omg"] = angular_search[0][0]
                            evaluation_result["max_neg_omg"] = angular_search[0][1]
                            evaluation_result["max_pos_alpa"] = angular_search[0][2]
                            evaluation_result["max_neg_alpa"] = angular_search[0][3]
                            evaluation_result["rapid_turn_acc"] = angular_search[0][4]
                            evaluation_result["rapic_turn_deacc"] = angular_search[0][5]
                            continue
                    elif module_name == "TrajectoryCheck":
                        traj_lateral_search = re.findall(trajectory_lateral_pattern, line)
                        if len(traj_lateral_search) > 0 and len(traj_lateral_search[0]) == 2:
                            evaluation_result["trajectory_lateral_diff_max"] = traj_lateral_search[0][0]
                            evaluation_result["trajectory_lateral_diff_medium"] = traj_lateral_search[0][1]
                            continue
                        traj_angle_search = re.findall(trajectory_angle_pattern, line)
                        if len(traj_angle_search) > 0 and len(traj_angle_search[0]) == 2:
                            evaluation_result["trajectory_angle_diff_max"] = traj_angle_search[0][0]
                            evaluation_result["trajectory_angle_diff_medium"] = traj_angle_search[0][1]
                            continue
        return evaluation_result




class RegressionRunner(object):
    def __init__(self, port="11311", vehicle=None, save_result=False, record_bag=False):
        self.ros_port = port
        self.conf = RegressionConfig()
        self.casefile = self.conf.get("regression_case", "caselist_filename")
        self.run_id = int(time.time())
        self.env = os.environ
        self.case_base = "cases"
        self.param_file = "planning.yaml"
        self.record_dir = self.conf.get("regression_case", "record_dir")
        if not os.path.exists(self.record_dir):
            os.makedirs(self.record_dir)
        self.vehicle = vehicle
        self.save_result = save_result
        self.record_bag = record_bag

        self.caselist = None
        self.get_caselist()


    def get_caselist(self):
        """
        read caselist
        for each case ,check description
        caselist format: test_{Issue key}.*
        """
        caselist = []
        with open(os.path.join("config",self.casefile),"r") as f:
            for line in f:
                caselist.append(line.strip("\n").strip())
        self.caselist = caselist



    def run(self):
        """
        run regression cases list in config
        record all bags by default
        """
        regression_result={}
        regression_exit_code = True

        failed_cases=[]
        failed_cases_descrption={}
        try:
            ### run regression case
            for case in self.caselist:
                case_dir = os.path.join(self.case_base, case)
                if not os.path.exists(case_dir):
                    print "Case not found :%s" % case_dir
                    continue
                print "Begin to run case: %s" % case_dir
                caserunner = CaseRunner(case_dir, self.record_dir,
                    port = self.ros_port,
                    vehicle = self.vehicle,
                    record=self.record_bag)
                caserunner.run()
                rospy.sleep(5)
                evaluation_result = caserunner.get_evaluation_result()
                case_result = caserunner.analyse_evaluation_result(evaluation_result)
                if case_result is False:
                    regression_exit_code = False
                    failed_cases.append(case)
                    failed_cases_descrption[case].append("Arrived: %s " %evaluation_result["arrived"])
                    failed_cases_descrption[case].append("Collided: %s " %evaluation_result["collided"])
                regression_result[case_dir] = evaluation_result
        except Exception as e:
            print e
        finally:
            pass
        print regression_result
        if self.save_result:
            test_result = TestResult()
            test_version = test_result.get_version(self.vehicle)
            test_result.save_evaluation_result(test_version, regression_result)
        if regression_exit_code == False:
            try:
                sys.exit(-1)
            except SystemExit as e:
                for i in failed_cases_descrption.items():
                    print i
                sys.exit(-1)


def test():
    #start roscore
    ros_port = "11311"
    if not os.path.exists('log'):
        os.mkdir('log')
    if ros_port is None:
        ros_port = "11311"
    os.environ["ROS_MASTER_URI"]="http://127.0.0.1:"+ros_port
    roscore = AWRoscore('log', os.environ, ros_port)
    roscore.run()
    roscore.init_check()
    ###init node
    rospy.init_node("runtimectrl", disable_signals=True)
    regressionrunner = RegressionRunner(save_result=True)
    regressionrunner.run()
    roscore.terminate()
    sys.exit(1)



def regression_args_parsing():
    parser = argparse.ArgumentParser(prog="regression",
                                     description="A tool for simulating test cases.")
    parser.add_argument("--save",action='store_true',
                        help="Save indication result to databases")
    parser.add_argument("--record",action='store_true',
                        help="Record bag to local, record dir is set in configuration, Notice: clean old bags manually!")
    parser.add_argument("--vehicleid", type=str, help="Run the cases by specific vehicle, eg arts16_3")
    parser.add_argument("--port", type=str, help="Run ros on specific port, eg 11311, default 11311")
    args = parser.parse_args()
    return args

def shutdown_fn():
    rospy.loginfo("node is shutting down")


if __name__ == "__main__":

    args = regression_args_parsing()
    save_result = args.save
    record = args.record
    vehicle = args.vehicleid
    ros_port = args.port
    start_time = time.time()
    start_time_str= time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(start_time))

    #start roscore
    if not os.path.exists('log'):
        os.mkdir('log')
    if ros_port is None:
        ros_port = "11311"
    os.environ["ROS_MASTER_URI"]="http://127.0.0.1:"+ros_port
    roscore = AWRoscore('log', os.environ, ros_port)
    roscore.run()
    roscore.init_check()
    ###init node
    rospy.init_node("runtimectrl", disable_signals=True)
    rospy.on_shutdown(shutdown_fn)

    # begin to run regression
    regressionrunner = RegressionRunner(save_result=save_result, record_bag=record, vehicle=vehicle, port=ros_port)
    regressionrunner.run()

    end_time = time.time()
    end_time_str = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(end_time))
    last = end_time-start_time

    # stop roscore
    roscore.terminate()
