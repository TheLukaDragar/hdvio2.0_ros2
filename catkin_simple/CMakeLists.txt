cmake_minimum_required(VERSION 3.5)
project(catkin_simple)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)

# This is a stub package for ROS2 compatibility
# catkin_simple is not needed in ROS2 as ament_cmake provides similar functionality

ament_package()
