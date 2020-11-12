#include "obstacle_restorer.h"

#include <unordered_set>
#include <string>
#include <future>
#include <fstream>
#include <thread>

using std::unordered_set;
using std::string;

namespace aw{
namespace simulation{

ObstacleRestorer::ObstacleRestorer(ros::NodeHandle nh)
       : nh_(nh) {
    LoadConfig();
           
    kGridPixelGranularity = kGridPixelStoreGranularity;

    kGridBlockNum1D = kRobotVisionRange / kGridBlockGranularity; 
    kGridBlockNum = kGridBlockNum1D * kGridBlockNum1D;

    kGridPixelNum1DPerBlock = kGridBlockGranularity / kGridPixelGranularity;
    kGridPixelNumPerBlock = kGridPixelNum1DPerBlock * kGridPixelNum1DPerBlock;

    kGridNum = kGridBlockNum * kGridPixelNumPerBlock;

    localized_pose_subscriber_ = nh_.subscribe(
            "/aw/localized_pose",
            kQueueSize,
            &ObstacleRestorer::LocalizedPoseCallback,
            this);

    fusion_map_true_publisher_ = nh.advertise<aw_idl::FusionMap>("/aw/fusion_map_true", 1);

    dynamic_obstacle_files_.clear();
    dynamic_obstacle_.clear();
    dynamic_obstacle_id_doing_.clear();
    dynamic_obstacle_id_to_do_.clear();

    fusion_map_step_ = 0;
    secs_= 0;
    
}

void ObstacleRestorer::LoadDynamicObstacleFromFile() {
    while (true) {
        mtx_data_.lock();
        //ROS_INFO_STREAM("1 mtx_data_.lock");
        if (loc_data_.empty()) {
            mtx_data_.unlock();
            //ROS_INFO_STREAM("1 mtx_data_.unlock");
            sleep(1);
            continue;
        }
     
        const aw_idl::LocalizedPose &localized_pose = loc_data_.top();
        std::stack<aw_idl::LocalizedPose> tmp;
        loc_data_.swap(tmp);
        mtx_data_.unlock();
        //ROS_INFO_STREAM("1 mtx_data_.unlock");

        double robot_utm_east = localized_pose.utm_east;
        double robot_utm_north = localized_pose.utm_north;
        
        std::vector<uint64_t> dynamic_obstacle_list;
        //gettimeofday(&start, NULL);
        /***dynamic_obstacle***/
        double offset = kRobotVisionRange;
        //double offset = kRobotVisionRange / 2;
        //load obstacle around the car 
        int64_t east_start_idx = std::floor((robot_utm_east - offset) / kGridBlockGranularity);
        int64_t north_start_idx = std::floor((robot_utm_north - offset) / kGridBlockGranularity);
        int64_t east_end_idx = std::ceil((robot_utm_east + offset) / kGridBlockGranularity);
        int64_t north_end_idx = std::ceil((robot_utm_north + offset) / kGridBlockGranularity);
        for (int64_t east_idx = east_start_idx; east_idx < east_end_idx; ++east_idx) {
            for (int64_t north_idx = north_start_idx; north_idx < north_end_idx; ++north_idx) {
                std::string path = std::to_string(east_idx) + "_" + std::to_string(north_idx);
                if (dynamic_obstacle_files_.find(path) != dynamic_obstacle_files_.end()) {
                    continue;
                } else {
                    dynamic_obstacle_files_.insert(path);
                }
                std::vector<std::string> files;
                path = kDynamicObstacleFilePath + '/' + path;
                GetFileList(path, files);
                for (const std::string & file : files) {
                    std::string file_name = path + "/" + file;
                    std::fstream input(file_name, std::ios::in);
                    if (!input.is_open()) {
                        ROS_ERROR_STREAM("Failed to open file!");
                        exit(0);
                    }
                    aw_simulation_obstacle::DynamicObstacleStateList state_list_;
                    if (!state_list_.ParseFromIstream(&input)) {
                        ROS_ERROR_STREAM("Failed to read in!");
                    }
                    dynamic_obstacle_[state_list_.id()] 
                        = std::make_shared<aw_simulation_obstacle::DynamicObstacleStateList>(state_list_);
                    dynamic_obstacle_list.push_back(state_list_.id());
                    input.close();
                    input.clear();
                }
            }
        }

        mtx_obstacle_.lock();
        //ROS_INFO_STREAM("1 mtx_obstacle_.lock");
        for (std::unordered_map<uint64_t, Pose>::iterator 
                it = dynamic_obstacle_id_to_do_.begin();
                it != dynamic_obstacle_id_to_do_.end();) {
            if (it->second.done == 2) {
                dynamic_obstacle_.erase(it->first);
                dynamic_obstacle_id_to_do_.erase(it++);
            } else {
                it++;
            }
        }
        for (auto id : dynamic_obstacle_list) {
            std::shared_ptr<aw_simulation_obstacle::DynamicObstacleStateList> state_list = dynamic_obstacle_[id];
            Pose pose;
            pose.utm_east = state_list->x();
            pose.utm_north = state_list->y();
            pose.yaw = state_list->yaw();
            pose.done = 0;
            dynamic_obstacle_id_to_do_[state_list->id()] = pose;
        }
        mtx_obstacle_.unlock();
        //ROS_INFO_STREAM("mtx_obstacle_.unlock");

        //gettimeofday(&end, NULL);
        //ROS_INFO_STREAM("timer4 = " << (1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)));
        sleep(1);
    }   
}


void ObstacleRestorer::InitFusionMap(
        const aw_idl::LocalizedPose &loc_pose,
        aw_idl::FusionMap & fp) {
    fp.header = loc_pose.header;
    fp.pose = loc_pose;
    fp.static_obstacle_grids.clear();
    fp.dynamic_obstacle_list.clear();
}

void ObstacleRestorer::ShiftTodoToDoing(const double robot_utm_east, 
                                        const double robot_utm_north, 
                                        const double robot_utm_yaw) {
    mtx_obstacle_.lock();
    for (std::unordered_map<uint64_t, Pose>::iterator it = dynamic_obstacle_id_to_do_.begin();
            it != dynamic_obstacle_id_to_do_.end(); 
            it++) {
        if (it->second.done != 0) {
            continue;
        }
        double num = 2 * M_PI;
        double num1 = (robot_utm_yaw - (int)(robot_utm_yaw/num) * num) + num;
        double remainder1 = num1 - (int)(num1/num) * num;
        double num2 = (it->second.yaw - (int)(it->second.yaw/num) * num) + num;
        double remainder2 = num2 - (int)(num2/num) * num;
        if (((pow(robot_utm_east - it->second.utm_east, 2) + pow(robot_utm_north - it->second.utm_north, 2)) < 1
            && abs(remainder1 - remainder2) < 1)
            || ((pow(robot_utm_east - it->second.utm_east, 2) + pow(robot_utm_north - it->second.utm_north, 2)) < 2
            && it->second.yaw < -99)) {
            if (dynamic_obstacle_[it->first]->dynamic_obstacle_state_size() > 2) {
                dynamic_obstacle_id_doing_[it->first].replay_times = kReplayTimesNotSet;
                dynamic_obstacle_id_doing_[it->first].current_pos = 1;
                dynamic_obstacle_id_doing_[it->first].total_size 
                        = (dynamic_obstacle_[it->first]->dynamic_obstacle_state_size() - 1);
                dynamic_obstacle_id_doing_[it->first].time_passed_stamp = 0;
                dynamic_obstacle_id_doing_[it->first].fusion_map_stamp = 0;
            }
            it->second.done = 1;
        }
    }
    mtx_obstacle_.unlock();
}

void ObstacleRestorer::ProcessDoingCreateDynamicObstacle(const aw_idl::LocalizedPose& localized_pose,
                                                        std::vector<uint64_t> &dynamic_obstacle_list) {
    // get the status of the robot
    double robot_utm_east = localized_pose.utm_east;
    double robot_utm_north = localized_pose.utm_north;
    double robot_utm_yaw = localized_pose.yaw;
    double sin_robot_utm_yaw = sin(robot_utm_yaw);
    double cos_robot_utm_yaw = cos(robot_utm_yaw);
    
    for (std::unordered_map<uint64_t, Iter>::iterator it = dynamic_obstacle_id_doing_.begin();
            it != dynamic_obstacle_id_doing_.end();) {
        aw_idl::DynamicObstacle obstacle;
        obstacle.id = it->first;
        
        double time_passed = fusion_map_step_ - (it->second.fusion_map_stamp - it->second.time_passed_stamp);
        it->second.time_passed_stamp += fusion_map_step_; //实际走过的时间　车辆的时间
        std::shared_ptr<aw_simulation_obstacle::DynamicObstacleStateList> state_list = dynamic_obstacle_[it->first];

        /******start*****/
        std::stack<aw_idl::LocalizedPose> loc_data_temp = loc_data_;
        std::vector<double> loc_data_temp_x;
        std::vector<double> loc_data_temp_y;

        int param1=5, param2=5;
        bool will_collide = false;

        aw_idl::LocalizedPose localized_pose_temp;
        for(int i=0;i<(std::min(param1, (int)loc_data_temp.size()));i++){
            localized_pose_temp = loc_data_temp.top();
            loc_data_temp.pop();
            loc_data_temp_x.push_back(localized_pose_temp.utm_east);
            loc_data_temp_y.push_back(localized_pose_temp.utm_north);
        }
        
        double A=0.0, B=0.0, C=0.0, D=0.0, E= 0.0, F = 0.0;
        for (uint32_t i=0; i<loc_data_temp_x.size(); i++){
                    A += loc_data_temp_x[i] * loc_data_temp_x[i];
                    B += loc_data_temp_x[i];
                    C += loc_data_temp_x[i] * loc_data_temp_y[i];
                    D += loc_data_temp_y[i];
        }
        double loc_data_a, loc_data_b, test_denominator = 0.0;
        if((test_denominator = (loc_data_temp_x.size()*A -B*B))){
            loc_data_a = (loc_data_temp_x.size() * C - B * D) / test_denominator;
            loc_data_b = (A * D - B * C) / test_denominator;
                
            // y = loc_data_a * x+loc_data_b
            loc_data_temp_x.clear();
            loc_data_temp_y.clear();
            ROS_INFO_STREAM("The line is:!!!!!!!");
            ROS_INFO_STREAM(loc_data_a);
            ROS_INFO_STREAM(loc_data_b);
            double robot_utm_east_temp =  localized_pose.utm_east;
            double robot_utm_north_temp = robot_utm_east_temp*loc_data_a + loc_data_b;
            
            double robot_utm_east_temp1 = robot_utm_east+5;
            double robot_utm_north_temp1 = robot_utm_east_temp1*loc_data_a + loc_data_b;

            //线段1 是(robot_utm_east_temp, robot_utm_north_temp) xa ya， (robot_utm_east_temp1, robot_utm_north_temp1) xb yb

            //线段2 是(east_, north_)xa ya, (east1_, north1_) xb yb
            double east_, north_, east1_, north1_, res1, res2, res3, res4;
            for(uint32_t i=it->second.current_pos; i<it->second.current_pos+param2; i++){
                east_ = state_list->dynamic_obstacle_state(i).robot_x();
                north_ = state_list->dynamic_obstacle_state(i).robot_y();
                east1_ = state_list->dynamic_obstacle_state(i+1).robot_x();
                north1_ = state_list->dynamic_obstacle_state(i+1).robot_y();
                if(std::max(robot_utm_east_temp1, robot_utm_east_temp) < std::min(east1_, east_) ||
                    std::max(east1_, east_) < std::min(robot_utm_east_temp1, robot_utm_east_temp) ||
                    std::max(robot_utm_north_temp1, robot_utm_north_temp) < std::min(north1_, north_) ||
                    std::max(north_, north_) < std::min(robot_utm_north_temp1, robot_utm_north_temp) ) {
                        will_collide =false;
                        continue;
                    }
                res1 = (robot_utm_east_temp - robot_utm_east_temp1) * (north_ - robot_utm_east_temp1) - (robot_utm_north_temp - robot_utm_north_temp1) * (east_ - robot_utm_east_temp1);
                res2 = (robot_utm_east_temp - robot_utm_east_temp1) * (north1_ - robot_utm_east_temp1) - (robot_utm_north_temp - robot_utm_north_temp1) * (east1_ - robot_utm_east_temp1);
                res3 = (east_ - east1_) * (robot_utm_north_temp - north1_) - (north_ - north1_) * (robot_utm_east_temp - east1_);
                res4 = (east_ - east1_) * (robot_utm_north_temp1 - north1_) - (north_ - north1_) * (robot_utm_east_temp1 - east1_);

                if(res1 * res2 <= 0 && res3 * res4 <= 0) {
                    will_collide = true;
                    break;
                }
            }
        } 
        else{
            loc_data_a = 1;
            loc_data_b = 0;
        }

        /*****end*****/

        while (it->second.current_pos < it->second.total_size 
               && time_passed > state_list->dynamic_obstacle_state(it->second.current_pos).nsecs()) {
            time_passed -= state_list->dynamic_obstacle_state(it->second.current_pos).nsecs();
            it->second.fusion_map_stamp += state_list->dynamic_obstacle_state(it->second.current_pos).nsecs(); // 障碍物的时间
            it->second.current_pos++;
        }

        const aw_simulation_obstacle::DynamicObstacleState & state 
                = state_list->dynamic_obstacle_state(it->second.current_pos);
        double x_utm_relative1 = state.robot_x();
        double y_utm_relative1 = state.robot_y();
        double step = dynamic_obstacle_[it->first]->dynamic_obstacle_state(it->second.current_pos).nsecs();

        double x_utm_relative2 
                = state_list->dynamic_obstacle_state(it->second.current_pos - 1).robot_x();
        double y_utm_relative2 
                = state_list->dynamic_obstacle_state(it->second.current_pos - 1).robot_y();

        double robot_x = (x_utm_relative1 - x_utm_relative2);
        double robot_y = (y_utm_relative1 - y_utm_relative2);
        //double pi_angle = atan2(robot_y, robot_x);
        
        x_utm_relative1 -= robot_utm_east;
        y_utm_relative1 -= robot_utm_north;
        x_utm_relative2 -= robot_utm_east;
        y_utm_relative2 -= robot_utm_north;
        robot_x = robot_x / step * time_passed + x_utm_relative2;
        robot_y = robot_y / step * time_passed + y_utm_relative2;
        double sin_robot_utm_yaw = sin(robot_utm_yaw);
        double cos_robot_utm_yaw = cos(robot_utm_yaw);
        obstacle.robot_x = robot_x * cos_robot_utm_yaw + robot_y * sin_robot_utm_yaw;
        obstacle.robot_y = robot_y * cos_robot_utm_yaw - robot_x * sin_robot_utm_yaw;
        obstacle.robot_z = state.robot_z();
        
        for (int i = 0; i < state.points_size(); i++) {
            double x = state.points(i).x() - robot_utm_east;
            double y = state.points(i).y() - robot_utm_north;
            geometry_msgs::Point32 p;
            p.x = x * cos_robot_utm_yaw + y * sin_robot_utm_yaw;
            p.y = y * cos_robot_utm_yaw - x * sin_robot_utm_yaw;
            p.z = state.points(i).z();
            obstacle.points.push_back(p);
        }

        if (state.polygon_size() > 0) {
            for (int i = 0; i < state.polygon_size(); i++) {
                double x = state.polygon(i).x() - robot_utm_east;
                double y = state.polygon(i).y() - robot_utm_north;
                geometry_msgs::Point32 p;
                p.x = x * cos_robot_utm_yaw + y * sin_robot_utm_yaw;
                p.y = y * cos_robot_utm_yaw - x * sin_robot_utm_yaw;
                p.z = state.polygon(i).z();
                obstacle.polygon.points.push_back(p);
            }
        } else {
            double pi_angle = atan2(state.relative_velo_y(), state.relative_velo_x());
            double cos_pi_angle = cos(pi_angle);
            double sin_pi_angle = sin(pi_angle);
            double other_robot_x = robot_x + 2 * cos_pi_angle + 1 * sin_pi_angle;
            double other_robot_y = robot_y - 1 * cos_pi_angle + 2 * sin_pi_angle;
            geometry_msgs::Point32 p;
            p.x = other_robot_x * cos_robot_utm_yaw + other_robot_y * sin_robot_utm_yaw;
            p.y = other_robot_y * cos_robot_utm_yaw - other_robot_x * sin_robot_utm_yaw;
            p.z = 0.0;
            obstacle.polygon.points.push_back(p);

            other_robot_x = robot_x - 2 * cos_pi_angle + 1 * sin_pi_angle;
            other_robot_y = robot_y - 1 * cos_pi_angle - 2 * sin_pi_angle;
            p.x = other_robot_x * cos_robot_utm_yaw + other_robot_y * sin_robot_utm_yaw;
            p.y = other_robot_y * cos_robot_utm_yaw - other_robot_x * sin_robot_utm_yaw;
            p.z = 0.0;
            obstacle.polygon.points.push_back(p);

            other_robot_x = robot_x - 2 * cos_pi_angle - 1 * sin_pi_angle;
            other_robot_y = robot_y + 1 * cos_pi_angle - 2 * sin_pi_angle;
            p.x = other_robot_x * cos_robot_utm_yaw + other_robot_y * sin_robot_utm_yaw;
            p.y = other_robot_y * cos_robot_utm_yaw - other_robot_x * sin_robot_utm_yaw;
            p.z = 0.0;
            obstacle.polygon.points.push_back(p);

            other_robot_x = robot_x + 2 * cos_pi_angle - 1 * sin_pi_angle;
            other_robot_y = robot_y + 1 * cos_pi_angle + 2 * sin_pi_angle;
            p.x = other_robot_x * cos_robot_utm_yaw + other_robot_y * sin_robot_utm_yaw;
            p.y = other_robot_y * cos_robot_utm_yaw - other_robot_x * sin_robot_utm_yaw;
            p.z = 0.0;
            obstacle.polygon.points.push_back(p);
        }
        double velo = hypot(state.relative_velo_x(), state.relative_velo_y());
        if (it->second.replay_times == kReplayTimesNotSet && velo > kStaticVeloThreshold) {
            it->second.replay_times = kDynamicReplayTimes;
        }
        double velo_x = state.relative_velo_x() * cos_robot_utm_yaw + state.relative_velo_y() * sin_robot_utm_yaw;
        double velo_y = state.relative_velo_y() * cos_robot_utm_yaw - state.relative_velo_x() * sin_robot_utm_yaw;
        double r_x = localized_pose.omega_yaw * obstacle.robot_y;
        double r_y = -localized_pose.omega_yaw * obstacle.robot_x;
        obstacle.relative_velo_x = velo_x - localized_pose.velo_robot_x + r_x;
        obstacle.relative_velo_y = velo_y - localized_pose.velo_robot_y + r_y; 
        obstacle.relative_velo_z = state.relative_acc_z();
        obstacle.relative_acc_x = state.relative_acc_x() - localized_pose.acc_robot_x;
        obstacle.relative_acc_y = state.relative_acc_y() - localized_pose.acc_robot_y;
        obstacle.relative_acc_z = state.relative_acc_z();
        obstacle.object_type = state.object_type();

        fusion_map_true_pub_.dynamic_obstacle_list.push_back(obstacle);

        if (it->second.current_pos >= it->second.total_size) {
            if (it->second.replay_times == kReplayTimesNotSet) {
                it->second.replay_times = kStaticReplayTimes;
            }
            it->second.replay_times--;
            if (it->second.replay_times > 0) {
                // replay
                it->second.current_pos = 0;
                it->second.time_passed_stamp = 0;
                it->second.fusion_map_stamp = 0;
            } else {
                //dynamic_obstacle_.erase(it->first);
                dynamic_obstacle_list.push_back(it->first);
                dynamic_obstacle_id_doing_.erase(it++);
            }
        } else {
            it++;
        }
    }

}

void ObstacleRestorer::Comp(
        const aw_idl::LocalizedPose& localized_pose) {
    if (secs_ == 0) {
        fusion_map_step_ = 0;
    } else {
        //accum time from the pre handled localized_pose;
        ros::Duration diff = localized_pose_.header.stamp - fusion_map_true_pub_.header.stamp;
        fusion_map_step_ = diff.toNSec();
    }

    InitFusionMap(localized_pose_, fusion_map_true_pub_);

    // get the status of the robot
    double robot_utm_east = localized_pose.utm_east;
    double robot_utm_north = localized_pose.utm_north;
    double robot_utm_yaw = localized_pose.yaw;
    double sin_robot_utm_yaw = sin(robot_utm_yaw);
    double cos_robot_utm_yaw = cos(robot_utm_yaw);
    
    ShiftTodoToDoing(robot_utm_east, robot_utm_north, robot_utm_yaw);

    std::vector<uint64_t> dynamic_obstacle_list;
    ProcessDoingCreateDynamicObstacle(localized_pose, dynamic_obstacle_list);
    mtx_obstacle_.lock();
    for (auto id : dynamic_obstacle_list) {
        std::unordered_map<uint64_t, Pose>::iterator iter = dynamic_obstacle_id_to_do_.find(id);
        iter->second.done = 2;
    }
    mtx_obstacle_.unlock();
}

void ObstacleRestorer::LocalizedPoseCallback(
        const aw_idl::LocalizedPose& localized_pose) {
    localized_pose_ = localized_pose;
    secs_ = localized_pose_.header.stamp.sec;
    nsecs_ = localized_pose_.header.stamp.nsec;
}

void ObstacleRestorer::run() {
    dynamic_obstacle_files_.clear();
    dynamic_obstacle_.clear();
    dynamic_obstacle_id_doing_.clear();
    dynamic_obstacle_id_to_do_.clear();
    secs_ = 0;
    nsecs_ = 0;
    fusion_map_step_ = 0;
    std::thread t(std::mem_fn(&ObstacleRestorer::LoadDynamicObstacleFromFile), this);
    t.detach();
    ros::Rate rate(kFusionMapFreq);
    while (ros::ok()) {
        mtx_data_.lock();
        loc_data_.push(localized_pose_);
        mtx_data_.unlock();
        Comp(localized_pose_);
        //load_invalid_area_.Load(localized_pose_, fusion_map_true_pub_);
        //load_curb_.Load(localized_pose_, fusion_map_true_pub_);
        fusion_map_true_publisher_.publish(fusion_map_true_pub_);
        ros::spinOnce();
        rate.sleep();
    }
}

}
}

        //线段1 是(robot_utm_east_, robot_utm_north_) xa ya， (esti_utm_east, esti_utm_north) xb yb

        //线段2 是(x_utm_relative1_, y_utm_relative1_)xa ya, (x_utm_relative2_, y_utm_relative2_) xb yb

        double res1, res2, res3, res4;

        if(std::max(robot_utm_east_, esti_utm_east) < std::min(x_utm_relative1_, x_utm_relative2_) ||
           std::max(x_utm_relative1_, x_utm_relative2_) < std::min(robot_utm_east_, esti_utm_east) ||
           std::max(robot_utm_north_, esti_utm_north) < std::min(y_utm_relative1_, y_utm_relative2_) ||
           std::max(y_utm_relative1_, y_utm_relative2_) < std::min(robot_utm_north_, esti_utm_north) ) {
               will_collide =false;
        } else{
            res1 = (robot_utm_east_ - esti_utm_east) * (y_utm_relative1_ - esti_utm_north) - (robot_utm_north_ - esti_utm_north) * (x_utm_relative1_ - esti_utm_east);
	        res2 = (robot_utm_east_ - esti_utm_east) * (y_utm_relative2_ - esti_utm_north) - (robot_utm_north_ - esti_utm_north) * (x_utm_relative2_ - esti_utm_east);
	        res3 = (x_utm_relative1_ - x_utm_relative2_) * (robot_utm_north_ - y_utm_relative2_) - (y_utm_relative1_ - y_utm_relative2_) * (robot_utm_east_ - x_utm_relative2_);
	        res4 = (x_utm_relative1_ - x_utm_relative2_) * (esti_utm_north - y_utm_relative2_) - (y_utm_relative1_ - y_utm_relative2_) * (esti_utm_east - x_utm_relative2_);

            if(res1 * res2 <= 0 && res3 * res4 <= 0) {
                will_collide = true;
                ROS_INFO_STREAM("*****************************************************GONNA COLLIDE");
            }
        }