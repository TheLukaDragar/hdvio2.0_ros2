cmake_minimum_required(VERSION 3.5)
project(svo_test_utils)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(svo_common REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)

include(SvoSetup)

################################################################################
set(HEADERS
  include/svo/test_utils/test_utils.h
  include/svo/test_utils/imu_simulation.h
  include/svo/test_utils/synthetic_dataset.h
  include/svo/test_utils/simple_raytracer.h
  include/svo/test_utils/trajectory_generator.h
  include/svo/test_utils/point_cloud_generator.h
)

set(SOURCES
  src/test_utils.cpp
  src/imu_simulation.cpp
  src/synthetic_dataset.cpp
  src/simple_raytracer.cpp
  src/trajectory_generator.cpp
  src/point_cloud_generator.cpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  cv_bridge
  ament_index_cpp
  svo_common
  vikit_ros
  vikit_common
  vikit_cameras
)
target_link_libraries(${PROJECT_NAME}
  glog::glog
)

################################################################################
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  cv_bridge
  eigen3_cmake_module
  Eigen3
  glog
  ament_index_cpp
  svo_common
  vikit_ros
  vikit_common
  vikit_cameras
)

ament_package()
