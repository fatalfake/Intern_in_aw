#ifndef AW_AW_SIMULATION_OBSTACLE_UTILS_H_
#define AW_AW_SIMULATION_OBSTACLE_UTILS_H_

#include <geometry_msgs/Point.h>
#include <string>
#include <vector>
#include <ros/ros.h>

namespace aw {
namespace simulation {

struct PointProb {
    uint16_t point;
    uint16_t prob;
};

extern std::string kFileAbsolutePath;
extern std::string kResultGridFilePath;
extern std::string kObstacleGridFilePath;
extern std::string kBaseGridFilePath;
extern std::string kBlockMapFilePath;
extern std::string kBlockMapGridFilePath;
extern std::string kStaticObstacleFilePath;
extern std::string kDynamicObstacleFilePath;
extern std::string kCurbFilePath;

extern int kQueueSize;

extern int kRobotHeight; 
extern int kRobotWidth; 
extern int kRobotLength;

extern int kRobotVisionForward;
extern int kRobotVisionBackward;
extern int kRobotVisionLeftRight;

extern int kRobotAccurateVisionForward;
extern int kRobotAccurateVisionBackward;
extern int kRobotAccurateVisionLeftRight;

extern int kRobotValidVisionForward;//40
extern int kRobotValidVisionBackward;//10
extern int kRobotValidVisionLeftRight;//20

extern int kRobotBlindVisionForward;//6
extern int kRobotBlindVisionBackward;//6
extern int kRobotBlindVisionLeftRight;//5

extern int kThreshold1;
extern int kThreshold2;
extern int kThreshold3;
extern int kDenominator;

// External param
extern int kRobotVisionRange;
extern double kRobotVisionGranularity;
// Interbal param
// 10m*10m storage block
extern int kGridBlockGranularity;
extern double kGridPixelGranularity;
extern double kGridPixelStoreGranularity;

extern int kThreadPoolSize;

extern uint32_t kGridBlockNum1D;
extern uint32_t kGridBlockNum;

extern uint32_t kGridPixelNum1DPerBlock;
extern uint32_t kGridPixelNumPerBlock;

extern uint32_t kGridNum;
extern int kFusionMapFreq;

extern double kStaticVeloThreshold;
extern int kDynamicReplayTimes;
extern int kStaticReplayTimes;

bool LoadConfig();
void unitest();
uint32_t RobotRelaCoor2GridIndex(
        double x,
        double y,
        double sin_robot_utm_yaw,
        double cos_robot_utm_yaw,
        double robot_diff_base_utm_east, 
        double robot_diff_base_utm_north);

geometry_msgs::Point GridIndex2RobotRelaCoor(
        uint32_t grid_index, 
        double sin_robot_utm_yaw,
        double cos_robot_utm_yaw,
        double robot_diff_base_utm_east, 
        double robot_diff_base_utm_north);

void LoadGridFromMultiFile(
        const std::string & file_path,
        uint32_t base_utm_east, 
        uint32_t base_utm_north,
        uint32_t * obstacle_grid);

void LoadGridFromFile(
        const std::string & filename,
        uint32_t * buff,
        const uint32_t num);

void DumpGridToMultiFile(
        uint32_t * obstacle_grid,
        const std::string & file_path,
        uint32_t base_utm_east, 
        uint32_t base_utm_north);

void DumpGridToFile(
        const uint32_t * buff,
        const uint32_t num,
        const std::string & filename);

int32_t rm_dir(const std::string& dir_full_path);
void CreateDirectory(const std::string& dir_full_path);
void DeleteDirectory(const std::string& dir_full_path);
void GetFileList(const std::string& file_path, std::vector<std::string> & files);

}
}

#endif
