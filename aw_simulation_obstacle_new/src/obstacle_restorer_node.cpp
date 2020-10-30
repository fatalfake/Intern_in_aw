#include <ros/ros.h>

#include "obstacle_restorer.h"
#include "utils.h"

int main(int argc, char ** argv) {
    ros::init(argc, argv, "obstacle_restorer");
    ros::NodeHandle nh;
    aw::simulation::ObstacleRestorer restorer(nh);
    restorer.run();
    
    ros::shutdown();   
    return 0;
}
