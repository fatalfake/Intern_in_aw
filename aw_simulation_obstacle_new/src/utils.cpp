#include "utils.h"

#include <cmath>
#include <cstdio>
#include <geometry_msgs/Point.h>
#include <ros/ros.h>
#include <dirent.h>
#include <sys/stat.h> 

namespace aw {
namespace simulation {

int kQueueSize;

std::string kFileAbsolutePath;
std::string kResultGridFilePath;
std::string kObstacleGridFilePath;
std::string kBaseGridFilePath;
std::string kBlockMapFilePath;
std::string kBlockMapGridFilePath;
std::string kStaticObstacleFilePath;
std::string kDynamicObstacleFilePath;
std::string kCurbFilePath;

int kRobotHeight; 
int kRobotWidth; 
int kRobotLength;

int kRobotVisionForward;
int kRobotVisionBackward;
int kRobotVisionLeftRight;

int kRobotAccurateVisionForward;
int kRobotAccurateVisionBackward;
int kRobotAccurateVisionLeftRight;

int kRobotValidVisionForward;//40
int kRobotValidVisionBackward;//10
int kRobotValidVisionLeftRight;//20

int kRobotBlindVisionForward;//6
int kRobotBlindVisionBackward;//6
int kRobotBlindVisionLeftRight;//5

int kThreshold1;
int kThreshold2;
int kThreshold3;
int kDenominator;

// External param
int kRobotVisionRange;
double kRobotVisionGranularity;
// Interbal param
// 10m*10m storage block
int kGridBlockGranularity;
double kGridPixelGranularity;
double kGridPixelStoreGranularity;

int kThreadPoolSize;

uint32_t kGridBlockNum1D;
uint32_t kGridBlockNum;

uint32_t kGridPixelNum1DPerBlock;
uint32_t kGridPixelNumPerBlock;

uint32_t kGridNum;

int kFusionMapFreq;

double kStaticVeloThreshold;
int kDynamicReplayTimes;
int kStaticReplayTimes;

bool LoadConfig() {
    if (!ros::param::get("~kQueueSize", kQueueSize)) {
        ROS_FATAL_STREAM("could not get param: ~kQueueSize");
        exit(1);
    }
    
    if (!ros::param::get("~kFileAbsolutePath", kFileAbsolutePath)) {
        ROS_FATAL_STREAM("could not get param: ~kFileAbsolutePath");
        exit(1);
    }
    if (!ros::param::get("~kResultGridFilePath", kResultGridFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kResultGridFilePath");
        exit(1);
    }
    kResultGridFilePath = kFileAbsolutePath + '/' + kResultGridFilePath;
    if (!ros::param::get("~kObstacleGridFilePath", kObstacleGridFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kObstacleGridFilePath");
        exit(1);
    }
    kObstacleGridFilePath = kFileAbsolutePath + '/' + kObstacleGridFilePath;
    if (!ros::param::get("~kBaseGridFilePath", kBaseGridFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kBaseGridFilePath");
        exit(1);
    }
    kBaseGridFilePath = kFileAbsolutePath + '/' + kBaseGridFilePath;
   
    if (!ros::param::get("~kRobotLength", kRobotVisionForward)) {
        ROS_FATAL_STREAM("could not get param: ~kBaseGridFilePath");
        exit(1);
    }
    if (!ros::param::get("~kRobotWidth", kRobotVisionBackward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionBackward");
        exit(1);
    }
    if (!ros::param::get("~kRobotHeight", kRobotVisionLeftRight)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionLeftRight");
        exit(1);
    }

    if (!ros::param::get("~kRobotVisionForward", kRobotVisionForward)) {
        ROS_FATAL_STREAM("could not get param: ~kBaseGridFilePath");
        exit(1);
    }
    if (!ros::param::get("~kRobotVisionBackward", kRobotVisionBackward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionBackward");
        exit(1);
    }
    if (!ros::param::get("~kRobotVisionLeftRight", kRobotVisionLeftRight)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionLeftRight");
        exit(1);
    }

    if (!ros::param::get("~kRobotValidVisionForward", kRobotValidVisionForward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotValidVisionForward");
        exit(1);
    }
    if (!ros::param::get("~kRobotValidVisionBackward", kRobotValidVisionBackward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotValidVisionBackward");
        exit(1);
    }
    if (!ros::param::get("~kRobotValidVisionLeftRight", kRobotValidVisionLeftRight)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotValidVisionLeftRight");
        exit(1);
    }

    if (!ros::param::get("~kRobotAccurateVisionForward", kRobotAccurateVisionForward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotAccurateVisionForward");
        exit(1);
    }
    if (!ros::param::get("~kRobotAccurateVisionBackward", kRobotAccurateVisionBackward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotAccurateVisionBackward");
        exit(1);
    }
    if (!ros::param::get("~kRobotAccurateVisionLeftRight", kRobotAccurateVisionLeftRight)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotAccurateVisionLeftRight");
        exit(1);
    }

    if (!ros::param::get("~kRobotBlindVisionForward", kRobotBlindVisionForward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotBlindVisionForward");
        exit(1);
    }
    if (!ros::param::get("~kRobotBlindVisionBackward", kRobotBlindVisionBackward)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotBlindVisionBackward");
        exit(1);
    }
    if (!ros::param::get("~kRobotBlindVisionLeftRight", kRobotBlindVisionLeftRight)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotBlindVisionLeftRight");
        exit(1);
    }

    if (!ros::param::get("~kThreshold1", kThreshold1)) {
        ROS_FATAL_STREAM("could not get param: ~kThreshold1");
        exit(1);
    }
    if (!ros::param::get("~kThreshold2", kThreshold2)) {
        ROS_FATAL_STREAM("could not get param: ~kThreshold2");
        exit(1);
    }
    if (!ros::param::get("~kThreshold3", kThreshold3)) {
        ROS_FATAL_STREAM("could not get param: ~kThreshold3");
        exit(1);
    }
    if (!ros::param::get("~kDenominator", kDenominator)) {
        ROS_FATAL_STREAM("could not get param: ~kDenominator");
        exit(1);
    }

    if (!ros::param::get("~kRobotVisionRange", kRobotVisionRange)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionRange");
        exit(1);
    }
    if (!ros::param::get("~kRobotVisionGranularity", kRobotVisionGranularity)) {
        ROS_FATAL_STREAM("could not get param: ~kRobotVisionGranularity");
        exit(1);
    }
    if (!ros::param::get("~kGridBlockGranularity", kGridBlockGranularity)) {
        ROS_FATAL_STREAM("could not get param: ~kGridBlockGranularity");
        exit(1);
    }
    if (!ros::param::get("~kGridPixelGranularity", kGridPixelGranularity)) {
        ROS_FATAL_STREAM("could not get param: ~kGridPixelGranularity");
        exit(1);
    }
    if (!ros::param::get("~kGridPixelStoreGranularity", kGridPixelStoreGranularity)) {
        ROS_FATAL_STREAM("could not get param: ~kGridPixelStoreGranularity");
        exit(1);
    }

    if (!ros::param::get("~kBlockMapFilePath", kBlockMapFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kBlockMapFilePath");
        exit(1);
    }
    if (!ros::param::get("~kBlockMapGridFilePath", kBlockMapGridFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kBlockMapGridFilePath");
        exit(1);
    }
    kBlockMapGridFilePath = kFileAbsolutePath + '/' + kBlockMapGridFilePath;

    if (!ros::param::get("~kThreadPoolSize", kThreadPoolSize)) {
        ROS_FATAL_STREAM("could not get param: ~kThreadPoolSize");
        exit(1);
    }

    if (!ros::param::get("~kStaticObstacleFilePath", kStaticObstacleFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kStaticObstacleFilePath");
        exit(1);
    }
    if (!ros::param::get("~kDynamicObstacleFilePath", kDynamicObstacleFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kDynamicObstacleFilePath");
        exit(1);
    }
    if (!ros::param::get("~kCurbFilePath", kCurbFilePath)) {
        ROS_FATAL_STREAM("could not get param: ~kCurbFilePath");
        exit(1);
    }
    
    if (!ros::param::get("~kFusionMapFreq", kFusionMapFreq)) {
        ROS_FATAL_STREAM("could not get param: ~kFusionMapFreq");
        exit(1);
    }

    if (!ros::param::get("~kStaticVeloThreshold", kStaticVeloThreshold)) {
        ROS_FATAL_STREAM("could not get param: ~kStaticVeloThreshold");
        exit(1);
    }
    if (!ros::param::get("~kStaticReplayTimes", kStaticReplayTimes)) {
        ROS_FATAL_STREAM("could not get param: ~kStaticReplayTimes");
        exit(1);
    }
    if (!ros::param::get("~kDynamicReplayTimes", kDynamicReplayTimes)) {
        ROS_FATAL_STREAM("could not get param: ~kDynamicReplayTimes");
        exit(1);
    }

    return true;
}


uint32_t RobotRelaCoor2GridIndex(
        double x,
        double y,
        double sin_robot_utm_yaw,
        double cos_robot_utm_yaw,
        double robot_diff_base_utm_east, 
        double robot_diff_base_utm_north) {
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x << "," << y);
    // point ralative to robot in robot => point relative to robot in utm
    double x_utm_relative = x * cos_robot_utm_yaw - y * sin_robot_utm_yaw;
    double y_utm_relative = y * cos_robot_utm_yaw + x * sin_robot_utm_yaw;
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x_utm_relative << "," << y_utm_relative);

    // => point relative to robot (but new zero) in utm
    double x_in_coordinate = x_utm_relative + robot_diff_base_utm_east; 
    double y_in_coordinate = y_utm_relative + robot_diff_base_utm_north;
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << "absolute utm" << (x_utm_relative + robot_utm_east) << "," << (y_utm_relative + robot_utm_north));
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x_in_coordinate << "," << y_in_coordinate);

    x_in_coordinate = round(x_in_coordinate / kGridPixelGranularity) * kGridPixelGranularity;
    y_in_coordinate = round(y_in_coordinate / kGridPixelGranularity) * kGridPixelGranularity;
    //x_in_coordinate = floor(x_in_coordinate / kGridPixelGranularity) * kGridPixelGranularity;
    //y_in_coordinate = floor(y_in_coordinate / kGridPixelGranularity) * kGridPixelGranularity;
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x_in_coordinate << "," << y_in_coordinate);

    // => block index
    uint32_t x_block_index = (x_in_coordinate / kGridBlockGranularity);
    uint32_t y_block_index = (y_in_coordinate / kGridBlockGranularity);
    uint32_t block_index = (x_block_index * (kGridBlockNum1D)) + y_block_index;
    //ROS_INFO_STREAM(x_block_index << "," << y_block_index << ":" << block_index);
   
    // => pixel index
    // m => cm mutiply 100
    uint32_t x_pixel_index = ((x_in_coordinate - x_block_index * kGridBlockGranularity) 
            / kGridPixelGranularity);
    uint32_t y_pixel_index = ((y_in_coordinate - y_block_index * kGridBlockGranularity) 
            / kGridPixelGranularity);
    uint32_t pixel_index = x_pixel_index * kGridPixelNum1DPerBlock + y_pixel_index;
    //ROS_INFO_STREAM(x_pixel_index << "," << y_pixel_index << ":" << pixel_index);

    // => grid index
    uint32_t grid_index = block_index * kGridPixelNumPerBlock + pixel_index;
    //ROS_INFO_STREAM(grid_index);
    return grid_index;
};

geometry_msgs::Point GridIndex2RobotRelaCoor(
        uint32_t grid_index, 
        double sin_robot_utm_yaw,
        double cos_robot_utm_yaw,
        double robot_diff_base_utm_east, 
        double robot_diff_base_utm_north) {
    //ROS_INFO_STREAM(grid_index);
    uint32_t pixel_index = grid_index % kGridPixelNumPerBlock;
    uint32_t x_pixel_index = pixel_index / kGridPixelNum1DPerBlock;
    uint32_t y_pixel_index = pixel_index % kGridPixelNum1DPerBlock;
    //ROS_INFO_STREAM(x_pixel_index << "," << y_pixel_index << ":" << pixel_index);

    uint32_t block_index = grid_index / kGridPixelNumPerBlock;
    uint32_t x_block_index = block_index / kGridBlockNum1D;
    uint32_t y_block_index = block_index % kGridBlockNum1D;
    //ROS_INFO_STREAM(x_block_index << "," << y_block_index << ":" << block_index);
    
    double x_in_coordinate = x_pixel_index * kGridPixelGranularity
        + x_block_index * kGridBlockGranularity;
    double y_in_coordinate = y_pixel_index * kGridPixelGranularity
        + y_block_index * kGridBlockGranularity; 
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x_in_coordinate << "," << y_in_coordinate);

    double x_utm_relative = x_in_coordinate - robot_diff_base_utm_east;
    double y_utm_relative = y_in_coordinate - robot_diff_base_utm_north;
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << "absolute utm" << (x_in_coordinate + base_utm_east) << "," << (y_in_coordinate + base_utm_north));
    //ROS_INFO_STREAM(std::setprecision(6) << std::fixed << x_utm_relative << "," << y_utm_relative);

    geometry_msgs::Point p;
    p.x = x_utm_relative * cos_robot_utm_yaw + y_utm_relative * sin_robot_utm_yaw;
    p.y = y_utm_relative * cos_robot_utm_yaw - x_utm_relative * sin_robot_utm_yaw;
    p.z = 1;
    //ROS_INFO_STREAM(p.x << "," << p.y);

    p.x = round(p.x / kRobotVisionGranularity) * kRobotVisionGranularity; 
    p.y = round(p.y / kRobotVisionGranularity) * kRobotVisionGranularity; 
    //ROS_INFO_STREAM(p.x << "," << p.y);

    return std::move(p);
}

void LoadGridFromMultiFile(
        const std::string & file_path,
        uint32_t base_utm_east, 
        uint32_t base_utm_north,
        uint32_t * obstacle_grid) {
    uint32_t * buff = obstacle_grid;
    for (uint32_t i = 0; i < kGridBlockNum; ++i) {
        uint32_t x = (i / kGridBlockNum1D) + base_utm_east;
        uint32_t y = (i % kGridBlockNum1D) + base_utm_north;
        std::string file_name = file_path + '/' + std::to_string(x) + '_' + std::to_string(y);
        LoadGridFromFile(file_name, buff, kGridPixelNumPerBlock);
        buff = buff +  kGridPixelNumPerBlock;
    }
}

void LoadGridFromFile(const std::string & file_name, uint32_t * buff, const uint32_t num) {
    FILE* fp = fopen(file_name.c_str(), "rb");
    if (fp == NULL) {
        //ROS_INFO_STREAM("open file " << file_name << " failed!");
        return;
    }
    size_t result = fread(buff, sizeof(uint32_t), num, fp);
    if (result != num) {
        ROS_FATAL_STREAM("read file " << file_name << " failed!");
        exit(1);
    }
    fclose(fp);
    fp = NULL;
}

void DumpGridToMultiFile(
        uint32_t * obstacle_grid,
        const std::string & file_path,
        uint32_t base_utm_east, 
        uint32_t base_utm_north) {
    uint32_t * buff = obstacle_grid;
    for (uint32_t i = 0; i < kGridBlockNum; ++i) {
        uint32_t x = (i / kGridBlockNum1D) + base_utm_east;
        uint32_t y = (i % kGridBlockNum1D) + base_utm_north;
        std::string file_name = file_path + '/' + std::to_string(x) + '_' + std::to_string(y);
        DumpGridToFile(buff, kGridPixelNumPerBlock, file_name);
        buff = buff + kGridPixelNumPerBlock;
    }
}

void DumpGridToFile(const uint32_t * buff, 
        const uint32_t num,
        const std::string & file_name) {
    FILE* fp = fopen(file_name.c_str(), "wb");
    if (fp == NULL) {
        ROS_FATAL_STREAM("open file " << file_name << " failed!");
        exit(1);
    }
    size_t result = fwrite(buff, sizeof(uint32_t), num, fp);
    if (result != num) {
        ROS_FATAL_STREAM("write file " << file_name << " failed!");
        exit(1);
    }
    fclose(fp);
    fp = NULL;
}

void DeleteDirectory(const std::string& dir_full_path) {
    DIR* dirp = opendir(dir_full_path.c_str());
    if (NULL != dirp) {
        ROS_INFO_STREAM("rm_dir" << dir_full_path);
        rm_dir(dir_full_path);
        closedir(dirp);
    }
}

int32_t rm_dir(const std::string& dir_full_path) {
    DIR* dirp = opendir(dir_full_path.c_str());    
    if (!dirp) {
        return -1;
    }
    struct dirent *dir;
    struct stat st;
    while ((dir = readdir(dirp)) != NULL) {
        if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) {
            continue;
        }    
        std::string sub_path = dir_full_path + '/' + dir->d_name;
        if (lstat(sub_path.c_str(),&st) == -1) {
            ROS_INFO_STREAM("rm_dir:lstat " << sub_path << " error");
            continue;
        }    
        if (S_ISDIR(st.st_mode)) {
            if (rm_dir(sub_path) == -1) {
                closedir(dirp);
                return -1;
            }
            rmdir(sub_path.c_str());
        } else if (S_ISREG(st.st_mode)) {
            unlink(sub_path.c_str());
        } else {
            ROS_INFO_STREAM("rm_dir:st_mode " << sub_path << " error");
            continue;
        }
    }
    if (rmdir(dir_full_path.c_str()) == -1) {
        closedir(dirp);
        return -1;
    }
    closedir(dirp);
    return 0;
}

void CreateDirectory(const std::string& dir_full_path) {
    DIR* dirp = opendir(dir_full_path.c_str());    
    if (NULL == dirp) {
        ROS_INFO_STREAM("mkdir " << dir_full_path);
        mkdir(dir_full_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    } else {
        closedir(dirp);
    }
}

void GetFileList(const std::string& file_path, std::vector<std::string> & files) {
    DIR *dir;
    class dirent *ent;
    class stat st;
    dir = opendir(file_path.c_str());
    if (NULL == dir) {
        //ROS_WARN("dir not exist");
        return;
        //exit(0);
    }
    while ((ent = readdir(dir)) != NULL) {
        const std::string file_name = ent->d_name;
        const std::string full_file_name = file_path + "/" + file_name;
        if (file_name[0] == '.') {
            continue;
        }
        if (stat(full_file_name.c_str(), &st) == -1) {
            continue;
        }
        const bool is_directory = (st.st_mode & S_IFDIR) != 0;
        if (is_directory) {
            continue;
        }
        files.push_back(file_name);
    }
    closedir(dir);
}

}
}
