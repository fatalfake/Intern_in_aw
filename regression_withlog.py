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
    def __init__(self, run_manager=None):
        self.run_manager = run_manager
        if run_manager is None:
            self.run_manager = RegressionManager()



    def get_version(self):
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
            if self.run_manager.vehicle is not None:
                if autowise_version != "":
                    autowise_version = autowise_version + "_" + self.run_manager.vehicle
                else:
                    autowise_version = self.run_manager.vehicle
        else:
            autowise_version = ""
            if self.run_manager.vehicle is not None:
                autowise_version = self.run_manager.vehicle
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


    def get_evaluation_result(self, case_dir):
        """
        解析每个case的evaluation结果
        返回dict
        如果结果文件缺失返回{}
        """
        result_file = os.path.join(case_dir, self.run_manager.evaluation_dir,
                                self.run_manager.evaluation_result_file)
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

    def format_detail_map(self, detail_map):
        detail_sorted = sorted(detail_map.items(),key=lambda x:x[0],reverse=False)
        retstr = ""
        mat = "{:20}\t{:28}"
        for k,v in detail_sorted:
            retstr += "\n"
            retstr += mat.format(k,v)
        return retstr


    def case_detail(self, case_key):
        """
        return the detail description of the case
        format is shown as belows
        """
        if case_key not in self.run_manager.case_descriptions:
            return "case info not found in description file"
        case_description = self.run_manager.case_descriptions[case_key]
        result_map  = {}
        result_map["title"] = case_description.getElementsByTagName("title")[0].childNodes[0].data
        result_map["link"] = "<a href={link}>{link}</a>".format(link=case_description.getElementsByTagName("link")[0].childNodes[0].data)
        result_map["assignee"] = case_description.getElementsByTagName("assignee")[0].childNodes[0].data
        result_map["created"] = case_description.getElementsByTagName("created")[0].childNodes[0].data
        result_map["updated"] = case_description.getElementsByTagName("updated")[0].childNodes[0].data
        customfields = case_description.getElementsByTagName("customfields")[0]
        customfields = customfields.getElementsByTagName("customfield")
        for field in customfields:


            fieldname = field.getElementsByTagName("customfieldname")[0].childNodes[0].data
            fieldvalues = field.getElementsByTagName("customfieldvalues")[0]
            fieldvalues = fieldvalues.getElementsByTagName("customfieldvalue")
            fieldvaluelist = []
            for customfieldvalue in fieldvalues:
                fieldvaluestr = customfieldvalue.childNodes[0].data
                fieldvaluestr.replace("]]>", "")
                fieldvaluestr.replace("<![CDATA[", "")
                fieldvaluelist.append(fieldvaluestr)
            result_map[fieldname] = "-".join(fieldvaluelist)

        return self.format_detail_map(result_map)

    def result_collect(self, start_time_str, end_time_str, last, save_result=False):
        result_info = {}
        test_version = self.get_version()


        report_title = u'接口测试报告'
        theme = u'接口数据统计'

        report_table_title = ['测试用例','Timeout设置', '执行结果',
                    '执行用时','到达', '碰撞', '与障碍物最小距离(cm)',
                    '与障碍物距离5分位(cm)', '与障碍物距离10分位(cm)', '详细信息']
        report_table_title_schema = ["case", "timeout_setting", "execution_result",
                    "execution_time", "arrived", "collided", "min_dist",
                    "quantile_5", "quantile_10", "detail"]

        case_detail = '''
TO BE DONE LATER
DETAIL OF THE CASE
'''
        report_cases = {"all_cases":[],
                        "error_cases":[],
                        "right_cases":[],
                        "untreaded_cases":[]}
        db_indication_result={}
        for case in self.run_manager.caselist:

            case_dir = os.path.join("cases", case)
            result_dir = os.path.join("cases", case, "result")
            result_file = "%s/running_%s.json" % (result_dir,
                                                self.run_manager.run_id)
            case_parts = case.split("_")

            # 默认的case result
            case_result = [case, "unkown", "未执行",
                    "0", "Failed", "False", "unknown",
                    "unknown", "unknown", "详细", case_detail]
            # 获取evaluation的结果
            evaluation_result = self.get_evaluation_result(case_dir)
            db_indication_result[case] = evaluation_result
            is_timeout = None
            arrived = None
            collided = None
            if "timeout" in evaluation_result:
                is_timeout = evaluation_result["timeout"]
            if "run_time" in evaluation_result:
                case_result[report_table_title_schema.index("execution_time")] = evaluation_result["run_time"]
            if "arrived" in evaluation_result:
                arrived = evaluation_result["arrived"]
                case_result[report_table_title_schema.index("arrived")] = arrived
            if "collided" in evaluation_result:
                collided = evaluation_result["collided"]
                case_result[report_table_title_schema.index("collided")] = collided
            if "min_dist" in evaluation_result:
                case_result[report_table_title_schema.index("min_dist")] = evaluation_result["min_dist"]
            if "quantile_5" in evaluation_result:
                case_result[report_table_title_schema.index("quantile_5")] = evaluation_result["quantile_5"]
            if "quantile_10" in evaluation_result:
                case_result[report_table_title_schema.index("quantile_10")] = evaluation_result["quantile_10"]


            # 获取case 描述, 放入详细信息中
            if len(case_parts) >=2 :
                case_key = case_parts[1]
                case_detail = self.case_detail(case_key)
                case_result[report_table_title_schema.index("detail") + 1] = case_detail


            timeout_setting = None
            # 未找到测试结果文件
            if not os.path.exists(result_file):
                result_info[case] = "run file not found"
            else:
                ## 获取 timeout 的设置时间
                with open(result_file, "r") as f:
                    json_str = f.readline()
                    run_result = json.loads(json_str)
                    if "setting" in run_result and "timeout" in run_result["setting"]:
                        timeout_setting =  run_result["setting"]["timeout"]
                        case_result[report_table_title_schema.index("timeout_setting")] = timeout_setting

            report_cases["all_cases"].append(case_result)
            if is_timeout is None or arrived is None or collided is None:
                report_cases["untreaded_cases"].append(case_result)
                self.regression_result = False
            else:
                if not is_timeout and arrived == "Success" and collided == "False":
                    case_result[report_table_title_schema.index("execution_result")] = "成功"
                    report_cases["right_cases"].append(case_result)
                else:
                    try:
                        case_result[report_table_title_schema.index("execution_result")] =  "失败"
                        report_cases["error_cases"].append(case_result)
                        self.regression_result = False
                        arrived_info = ("Arrived: %s" %arrived)
                        collided_info = ("Collided: %s" %collided)
                        raise caseError(-1, '检测到case失败', case, arrived_info, collided_info,)
                    except caseError as e:
                        print e.message
                        print e

        # 保存evaluation的结果到数据库
        if save_result:
            self.save_evaluation_result(test_version, db_indication_result)
        test_info = [start_time_str, end_time_str, last, len(report_cases['all_cases']), len(report_cases['right_cases']) + len(report_cases['error_cases']), len(report_cases['untreaded_cases'])]
        test_info.append(test_version)
        html_report_object = HTML_REPORT(report_title, theme, test_info, report_table_title, report_cases)
        html = html_report_object.genHtmlReport()
        with open("test_demo.html",'wb') as f:
            f.write(html.encode('utf-8'))

        return result_info

    def print_result(self, result_info):
        print "%s Test Result %s" % ("="*20, "=" * 20 )
        for each in result_info:
            print "%s :%s" % (each, result_info[each])

class caseError(Exception):
    def __init__(self, code = -1, message = '检测到case失败', *args):
        self.args = args
        self.message = message
        self.code = code


class RegressionManager(object):
    def __init__(self, port="11311", vehicle=None):
        self.ros_port = port
        self.conf = RegressionConfig()
        self.casefile = self.conf.get("regression_case", "caselist_filename")
        self.run_id = int(time.time())
        self.casedescription_file = self.conf.get("case_description", "description_file")
        self.casesemicolon = self.conf.get("case_description", "file_semicolon")
        self.env = os.environ
        self.case_base = "cases"
        self.param_file = "planning.yaml"
        self.vehicle = vehicle

        #### evaluation
        self.evaluation_param_name = "/aw/evaluation/conf_path"
        self.evaluation_dir = "evaluation"
        self.evaluation_conf_file = "conf.yaml"
        self.evaluation_result_file = "result.txt"
        ###init
        self.case_descriptions = None
        self.format_case_description(self.casedescription_file, self.casesemicolon)
        ####init
        self.caselist = None
        self.get_caselist()
        self.regression_result = True


    def format_case_description(self, filename, semicolon="|"):
        """
        get case info from csv file
        regression initiate
        """
        self.case_descriptions = {}
        DOMTree = xml.dom.minidom.parse(os.path.join("config", filename))
        collection = DOMTree.documentElement
        channellist = collection.getElementsByTagName("channel")
        if len(channellist)  <= 0:
            sys.exit(0)
        cases =  channellist[0].getElementsByTagName("item")
        for case in cases:
            key = case.getElementsByTagName("key")
            if len(key) > 0:
                case_key = key[0].childNodes[0].data
                self.case_descriptions[case_key] = case


    def __clean_paramserver(self):
        params = list_params("/")
        for param in params:
            delete_param(param, True)


    def __init_nodeyaml(self, casedir):
        yamlfile = os.path.join(casedir, self.param_file)
        if not os.path.exists(yamlfile):
            print("%s not found , should regenerate the case"%yamlfile)
            sys.exit(1)
        data = read_yaml(yamlfile)
        try:
            evaluation_conf_dir = os.path.join(casedir, self.evaluation_dir)
            if not os.path.exists(evaluation_conf_dir):
                os.mkdir(evaluation_conf_dir)
            evaluation_conf_yaml_file = os.path.join(evaluation_conf_dir, self.evaluation_conf_file)
            if not os.path.exists(evaluation_conf_yaml_file):
                os.system("cp config/evaluation/conf.yaml %s"%evaluation_conf_dir)
            data[self.evaluation_param_name] = os.path.abspath(evaluation_conf_dir)

            for k in data['obstacle_restorer_node']:
                s = data['obstacle_restorer_node'][k]
                p = os.path.basename(s)
                c = os.path.abspath(casedir)
                data['obstacle_restorer_node'][k] = os.path.join(c, p)
        except KeyError as err:
            rospy.logerr(
                err.message + " missed in the 'planning.yaml' of that case.")
            raise err
        with open(yamlfile, 'w') as f:
            yaml.dump(data, f)
        return data


    def __loadparam_main(self, casedir, vehicleid_param):
        os.environ["ROS_MASTER_URI"] = "http://127.0.0.1:" + self.ros_port
        os.environ["ROS_IP"] = "127.0.0.1"
        self.__clean_paramserver()
        params = self.__init_nodeyaml(casedir)
        try:
            if "vehicle_id" in params["aw_simulation_vehicle"]:
                vehicle_id = params["aw_simulation_vehicle"]["vehicle_id"]
                print("vhicle id: %s " % vehicle_id)
            else:
                vehicle_id = params["vehicle_pose_collector"]["vehicle_id"]
            ###制定了仿真的车辆
            if vehicleid_param is not None:
                vehicle_id = vehicleid_param
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
        node_yaml_file = os.path.join(casedir, self.param_file)
        with open(node_yaml_file) as f:
            node = load(f)
        upload_params("/", node)

        with open(casedir+'/goal.yaml', 'r') as f:
            yamlfile = load(f)
        upload_params('/', yamlfile)
        #Pload = subprocess.Popen("rosparam load %s" % node_yaml_file, shell=True)
        return True





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

    def run_case(self, case_dir, vehicle, ctrl, stdout, stderr):
        """
        run specific case
        specify the stdout and stderr
        privides running conf
        """
        if not os.path.exists(case_dir):
            logging.error("%s not found in cases"%case_dir)
            return
        else:
            logging.info(" %s Begin to run %s %s"% ("=" * 10, case_dir, "=" * 10))
        # load environment and param

        rosmap_cmd = "roslaunch  aw_hdmap hdmap_runtime_env.launch"
        rosevaluation_cmd = "roslaunch aw_evaluation start_evaluation.launch"
        rossim_cmd = "roslaunch launch/run_case_simulation_by_env.launch"

        self.__loadparam_main(case_dir, vehicle)
        env = os.environ
        rosmap = AWProcess(rosmap_cmd, None, stdout, stderr, env)
        rossim = AWProcess(rossim_cmd, None, stdout, stderr, env)
        rosevaluation = AWProcess(rosevaluation_cmd, None, stdout, stderr, env)

        if rospy.is_shutdown():
            rospy.loginfo("shuting down already")

        rosmap.run()
        rospy.sleep(3)
        ## launch evaluation ahead in case something missed at beginning
        rosevaluation.run()
        rospy.sleep(3)
        rossim.run()

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
            rosnode.kill_nodes(['/hdmap_node','/hdmap'])
            rossim.terminate()
            rosmap.terminate()
            rosevaluation.terminate()


    def run_regression(self, vehicle=None):
        """
        run regression cases list in config
        """
        try:
            ### run regression case
            for case in self.caselist:
                case_dir = os.path.join(self.case_base, case)
                if not os.path.exists(case_dir):
                    print "Case not found :%s" % case_dir
                    continue
                print "Begin to run case: %s" % case_dir
                SIMU_LOG_FILE = "simu_when%d.log" % int(self.run_id)
                SIMU_LOG_FILE = os.path.join(case_dir, SIMU_LOG_FILE)
                logf = open(SIMU_LOG_FILE, 'a')
                ctrl = RuntimeManager(autopause=False, outputpath=None, dtask=True,
                              autoexit=True, casename=case_dir, timeout=None,runid=self.run_id, enable_keyboard=False)
                self.run_case(case_dir, vehicle, ctrl, logf, logf)

        except Exception as e:
            print e
        finally:
            ###roscore stop
            roscore.terminate()




    def update_conf(self):
        self.conf.write_conf("regression_case", "case_running_id", str(self.run_id))

def shutdown_fn():
    rospy.loginfo("node is shutting down")


def test_report_func():
    rospy.init_node("runtimectrl", disable_signals=True)
    result = TestResult()
    #print result.case_detail("JIADING-186")
    #result.result_collect("123","345",123123)
    print result.get_evaluation_result("cases/test_JIADING-196")
    sys.exit(0)

def regression_args_parsing():
    parser = argparse.ArgumentParser(prog="regression",
                                     description="A tool for simulating test cases.")
    parser.add_argument("--save",action='store_true',
                        help="Save indication result to databases")
    parser.add_argument("--vehicleid", type=str, help="Run the cases by specific vehicle, eg arts16_3")
    parser.add_argument("--port", type=str, help="Run ros on specific port, eg 11311, default 11311")
    args = parser.parse_args()
    return args


#test_report_func()

if __name__ == "__main__":

    args = regression_args_parsing()
    save_result = args.save
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
    run_manager = RegressionManager(ros_port, vehicle)

    run_manager.run_regression(vehicle)
    # update id in config file
    run_manager.update_conf()

    end_time = time.time()
    end_time_str = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(end_time))
    last = end_time-start_time
    # generate report
    result = TestResult(run_manager)
    result_info = result.result_collect(start_time_str, end_time_str, last, save_result)
    result.print_result(result_info)

    # stop roscore
    roscore.terminate()
    print run_manager.regression_result
    if run_manager.regression_result:
        sys.exit(0)
    else:
        sys.exit(1)
