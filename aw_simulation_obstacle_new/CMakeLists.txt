cmake_minimum_required(VERSION 2.8.3)
project(aw_simulation_obstacle)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++14 -O2 -Wall -Wno-unused)

include_directories(
  proto
  /opt/ros/kinetic/include 
  /home/autowise/workspace/autowise/devel/include 
)

add_executable(obstacle_restorer_node src/obstacle_restorer.cpp src/obstacle_restorer_node.cpp src/utils.cpp proto/dynamic_obstacle.pb.cc) 

target_link_libraries(obstacle_restorer_node
  -L/usr/local/lib -lprotobuf
  -L/opt/ros/kinetic/lib -lroscpp -lrosconsole -lroscpp_serialization -lrostime -lcpp_common
  -lpthread
)

