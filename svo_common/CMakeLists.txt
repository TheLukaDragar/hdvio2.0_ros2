cmake_minimum_required(VERSION 3.5)
project(svo_common)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(fast_neon REQUIRED)
find_package(rclcpp REQUIRED)
find_package(glog REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(eigen_checks REQUIRED)

include(SvoSetup)

set(HEADERS
  include/svo/common/camera.h
  include/svo/common/camera_fwd.h
  include/svo/common/feature_wrapper.h
  include/svo/common/frame.h
  include/svo/common/imu_calibration.h
  include/svo/common/logging.h
  include/svo/common/occupancy_grid_2d.h
  include/svo/common/point.h
  include/svo/common/seed.h
  include/svo/common/transformation.h
  include/svo/common/types.h
  )

set(SOURCES
  src/frame.cpp
  src/point.cpp
  )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  vikit_common
  vikit_cameras
  fast_neon
  rclcpp
  eigen_checks
)
target_link_libraries(${PROJECT_NAME}
  glog::glog
)

################################################################################
# TESTS

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(${PROJECT_NAME}-test
    test/test_main.cpp
    test/test_feature_wrapper.cpp
  )
  target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})

  ament_add_gtest(container-helpers-test
    test/test_container_helpers.cpp
  )
  target_link_libraries(container-helpers-test ${PROJECT_NAME})
endif()

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
  vikit_common
  vikit_cameras
  fast_neon
  glog
  eigen3_cmake_module
  Eigen3
  eigen_checks
)

ament_package()
