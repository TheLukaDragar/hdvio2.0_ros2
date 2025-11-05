cmake_minimum_required(VERSION 3.5)
project(svo_ceres_backend)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_vio_common REQUIRED)
find_package(svo REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)
find_package(Ceres REQUIRED)
find_package(CUDA REQUIRED)
find_package(yaml-cpp REQUIRED)

set(TRT_ROOT $ENV{TRT_ROOT})

include(SvoSetup)

# OpenMP
find_package(OpenMP QUIET)
if(OPENMP_FOUND)
  message(STATUS "OpenMP found")
  add_definitions(-DUSE_OPENMP)
endif()

#############
# LIBRARIES #
#############
set(HEADERS
  include/svo/ceres_backend_interface.hpp
  include/svo/ceres_backend/estimator.hpp
  include/svo/ceres_backend/estimator_impl.hpp
  include/svo/ceres_backend/estimator_types.hpp
  include/svo/ceres_backend/ceres_iteration_callback.hpp
  include/svo/ceres_backend/error_interface.hpp
  include/svo/ceres_backend/homogeneous_point_error.hpp
  include/svo/ceres_backend/homogeneous_point_local_parameterization.hpp
  include/svo/ceres_backend/homogeneous_point_parameter_block.hpp
  include/svo/ceres_backend/imu_error.hpp
  include/svo/ceres_backend/dynamics_error.hpp
  include/svo/ceres_backend/dynamics_error_vid.hpp
  include/svo/ceres_backend/dynamics_error_hdviopp.hpp
  include/svo/ceres_backend/dynamics_error_hdviopp_zeromean_force_prior.hpp
  include/svo/ceres_backend/local_parameterization_additional_interfaces.hpp
  include/svo/ceres_backend/map.hpp
  include/svo/ceres_backend/marginalization_error.hpp
  include/svo/ceres_backend/marginalization_error_impl.hpp
  include/svo/ceres_backend/parameter_block.hpp
  include/svo/ceres_backend/pose_error.hpp
  include/svo/ceres_backend/pose_local_parameterization.hpp
  include/svo/ceres_backend/pose_parameter_block.hpp
  include/svo/ceres_backend/relative_pose_error.hpp
  include/svo/ceres_backend/reprojection_error.hpp
  include/svo/ceres_backend/reprojection_error_impl.hpp
  include/svo/ceres_backend/reprojection_error_base.hpp
  include/svo/ceres_backend/speed_and_bias_error.hpp
  include/svo/ceres_backend/speed_and_bias_parameter_block.hpp
  include/svo/ceres_backend/external_force_parameter_block.hpp
  include/svo/ceres_backend/map_alignment.hpp
  include/svo/ceres_backend/epipolar_error.hpp
  include/svo/ceres_backend/reprojection_error_simple.hpp
  include/svo/motion_detector.hpp
  include/svo/outlier_rejection.hpp
  include/svo/ceres_backend_publisher.hpp
  )

set(SOURCES
  src/ceres_backend_interface.cpp
  src/estimator.cpp
  src/error_interface.cpp
  src/homogeneous_point_error.cpp
  src/homogeneous_point_parameter_block.cpp
  src/homogeneous_point_local_parameterization.cpp
  src/imu_error.cpp
  src/dynamics_error.cpp
  src/dynamics_error_vid.cpp
  src/dynamics_error_hdviopp.cpp
  src/dynamics_error_hdviopp_zeromean_force_prior.cpp
  src/local_parameterization_additional_interfaces.cpp
  src/map.cpp
  src/marginalization_error.cpp
  src/pose_error.cpp
  src/pose_parameter_block.cpp
  src/pose_local_parameterization.cpp
  src/relative_pose_error.cpp
  src/speed_and_bias_error.cpp
  src/speed_and_bias_parameter_block.cpp
  src/external_force_parameter_block.cpp
  src/motion_detector.cpp
  src/outlier_rejection.cpp
  src/ceres_backend_publisher.cpp
  src/map_alignment.cpp
  )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
  ${CERES_INCLUDE_DIRS}
  ${CUDA_INCLUDE_DIRS}
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  geometry_msgs
  sensor_msgs
  svo_common
  svo_vio_common
  svo
  pcl_conversions
)

target_link_libraries(${PROJECT_NAME}
  ${CERES_LIBRARIES}
  glog::glog
  yaml-cpp
  ${CUDA_LIBRARIES}
  ${CUDA_CUBLAS_LIBRARIES}
)

##########
# GTESTS #
##########
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  
  ament_add_gtest(test_backend_id test/test_backend_id.cpp)
  target_link_libraries(test_backend_id ${PROJECT_NAME})
  
  ament_add_gtest(test_estimator test/test_estimator.cpp)
  target_link_libraries(test_estimator ${PROJECT_NAME})
  
  ament_add_gtest(test_homogeneous_point_error test/test_homogeneous_point_error.cpp)
  target_link_libraries(test_homogeneous_point_error ${PROJECT_NAME})
  
  ament_add_gtest(test_map test/test_map.cpp)
  target_link_libraries(test_map ${PROJECT_NAME})
  
  ament_add_gtest(test_marginalization test/test_marginalization.cpp)
  target_link_libraries(test_marginalization ${PROJECT_NAME})
  
  ament_add_gtest(test_pose_error test/test_pose_error.cpp)
  target_link_libraries(test_pose_error ${PROJECT_NAME})
  
  ament_add_gtest(test_pose_local_parameterization test/test_pose_local_parameterization.cpp)
  target_link_libraries(test_pose_local_parameterization ${PROJECT_NAME})
  
  ament_add_gtest(test_reprojection_error test/test_reprojection_error.cpp)
  target_link_libraries(test_reprojection_error ${PROJECT_NAME})
  
  ament_add_gtest(test_relative_pose_error test/test_relative_pose_error.cpp)
  target_link_libraries(test_relative_pose_error ${PROJECT_NAME})
  
  ament_add_gtest(test_speed_and_bias_error test/test_speed_and_bias_error.cpp)
  target_link_libraries(test_speed_and_bias_error ${PROJECT_NAME})
  
  ament_add_gtest(test_imu_error test/test_imu_error.cpp)
  target_link_libraries(test_imu_error ${PROJECT_NAME})
endif()

##########
# EXPORT #
##########
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  svo_common
  svo_vio_common
  svo
  pcl_ros
  Eigen3
)
ament_package()
