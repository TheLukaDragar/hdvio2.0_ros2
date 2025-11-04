cmake_minimum_required(VERSION 3.5)
project(svo_benchmarking)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosbag2_cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# Install Python modules
ament_python_install_package(svo_ceres_benchmarking
  PACKAGE_DIR src/svo_ceres_benchmarking
)

# Install scripts directory
install(DIRECTORY scripts/
  DESTINATION lib/${PROJECT_NAME}
  USE_SOURCE_PERMISSIONS
  PATTERN "*.sh" EXCLUDE
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
