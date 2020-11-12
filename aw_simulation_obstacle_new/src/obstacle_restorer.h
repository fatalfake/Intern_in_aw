#ifndef AW_AW_SIMULATION_OBSTACLE_RESTORER_NEW_H_
#define AW_AW_SIMULATION_OBSTACLE_RESTORER_NEW_H_

#include <ros/ros.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <aw_idl/FusionMap.h>
#include <stdio.h>
#include <sys/time.h>
#include <mutex>
#include <stack>
#include <memory> 
#include "aw_simulation_obstacle/RestoreObstacle.h"
#include "utils.h"
#include "dynamic_obstacle.pb.h"
#include <mutex>

#include "geometry_msgs/Point.h"

namespace aw {
namespace simulation {

typedef bool (*FunType) (double, double);

class ObstacleRestorer {
public:
    explicit ObstacleRestorer(ros::NodeHandle nh);
    ~ObstacleRestorer() {};
    void run();

private:
    bool RestoreObstacleCallback(
            aw_simulation_obstacle::RestoreObstacle::Request& req,
            aw_simulation_obstacle::RestoreObstacle::Response& res);

    void LocalizedPoseCallback(
        const aw_idl::LocalizedPose& localized_pose);

    void Comp(
        const aw_idl::LocalizedPose& localized_pose);

    ros::NodeHandle nh_;

    ros::ServiceServer restore_obstacle_server_;
    
    ros::Publisher fusion_map_true_publisher_;
    
    ros::Subscriber localized_pose_subscriber_;

    aw_idl::LocalizedPose localized_pose_;

    aw_idl::FusionMap fusion_map_true_;
    uint32_t* fusion_map_true_seg_size_;
    
    aw_idl::FusionMap fusion_map_true_pub_;
    
    //LoadInvalidArea load_invalid_area_;
    //LoadCurb load_curb_;

     static constexpr int kReplayTimesNotSet = -100;

private:
    /*dynamic obstacle*/
    std::set<std::string> dynamic_obstacle_files_;
    std::unordered_map<uint64_t, std::shared_ptr<aw_simulation_obstacle::DynamicObstacleStateList>> dynamic_obstacle_;
    
    struct Iter {
        int32_t replay_times;
        uint32_t current_pos;
        uint32_t total_size;
        uint64_t time_passed_stamp;
        uint64_t fusion_map_stamp;
    };
    //id, current, size
    std::unordered_map<uint64_t, Iter> dynamic_obstacle_id_doing_;
    
    struct Pose {
        uint32_t done;
        double utm_east;
        double utm_north;
        double yaw;
    };
    //id, utm_east, utm_north
    std::unordered_map<uint64_t, Pose> dynamic_obstacle_id_to_do_;

    uint64_t secs_;
    uint64_t nsecs_;
    uint64_t fusion_map_step_;
    std::mutex mtx_obstacle_;
    std::stack<aw_idl::LocalizedPose> loc_data_;
    std::mutex mtx_data_;

private:
    void InitFusionMap(
        const aw_idl::LocalizedPose &loc_pose,
        aw_idl::FusionMap & fp);

    void FillFusionMap(
        const uint32_t block_id,
        const double block_base_utm_east,
        const double block_base_utm_north);

    void LoadDynamicObstacleFromFile();
  
    void ShiftTodoToDoing(const double robot_utm_east, 
                          const double robot_utm_north, 
                          const double robot_utm_yaw);

    void ProcessDoingCreateDynamicObstacle(const aw_idl::LocalizedPose &localized_pose, 
                                           std::vector<uint64_t> &dynamic_obstacle_list);

};
}
}

#endif
