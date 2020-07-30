#!/usr/bin/python
# -*- coding:utf-

import rospy
import yaml
import os
import rosparam
import sys
from regression import RegressionManager
from simlib.runtime_manager import RuntimeManager
from simlib.aw_roscore import AWRoscore

import subprocess
import logging
import argparse
import time
import rosnode
from rosparam import upload_params
from yaml import load

def args_parsing():
    parser = argparse.ArgumentParser(prog="simu",
                                     description="A tool for simulating test cases.")
    parser.add_argument("case_dir", type=str, nargs='?', default="test/",
                        help="The case directory to run. Default to 'test/'.")
    parser.add_argument('--timeout', type=float, metavar='SEC',
                        help="The timeout to exit or pause automatically.")
    parser.add_argument('--pause', action='store_true',
                        help='Whether to auto pause after timeout or ending point.')
    parser.add_argument('--exit', action='store_true',
                        help='Whether to auto exit after timeout or ending point.')
    parser.add_argument('--output', type=str, help='Where to put the output json.')
    parser.add_argument('--difftasks', action='store_true',
                        help='Indicating not checking task_idx when running.')
    parser.add_argument("--vehicleid", type=str, help="Run the case by specific vehicle, eg arts16_3")
    parser.add_argument("--port", type=str, help="Run ros on specific port, eg 11311, default 11311")
    parser.add_argument("--run_id", type=int, help="Get the same run_id from regression")
    args = parser.parse_args()

    return args


if __name__ == "__main__":
    logging.basicConfig(
        format='%(asctime)s:%(levelname)s:%(message)s', level=logging.DEBUG)
    args = args_parsing()
    case_dir = args.case_dir
    vehicle = args.vehicleid
    ros_port = args.port
    run_id = args.run_id
    SIMU_LOG_FILE = "simu_when%d.log" % int(run_id)
    RESULT_FILE = "result.yaml"
    SIMU_LOG_FILE = os.path.join(case_dir, SIMU_LOG_FILE)
    RESULT_FILE = os.path.join(case_dir, RESULT_FILE)
    logf = open(SIMU_LOG_FILE, 'a')

    if not os.path.exists('log'):
        os.mkdir('log')
    os.environ["ROS_MASTER_URI"] = "http://127.0.0.1:" + ros_port
    roscore = AWRoscore('log', os.environ, ros_port)
    roscore.run()
    roscore.init_check()

    rospy.init_node("runtimectrl", disable_signals=True)
    ctrl = RuntimeManager(autopause=args.pause, outputpath=args.output, dtask=args.difftasks,
                                  autoexit=args.exit, casename=case_dir, timeout=args.timeout, runid=run_id, enable_keyboard=False)

    run_manager = RegressionManager(ros_port)
    run_manager.run_case(case_dir,vehicle, ctrl, logf, logf)
    roscore.terminate()



