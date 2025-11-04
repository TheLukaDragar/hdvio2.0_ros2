cmake_minimum_required(VERSION 3.5)
project(vikit_common)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(minkindr REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(glog REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)

include(SvoSetup)

set(HEADERS
  include/vikit/blender_utils.h
  include/vikit/math_utils.h
  include/vikit/performance_monitor.h
  include/vikit/ringbuffer.h
  include/vikit/sample.h
  include/vikit/timer.h
  include/vikit/user_input_thread.h
  include/vikit/vision.h
  include/vikit/homography_decomp.h
  include/vikit/homography.h
  include/vikit/path_utils.h
  include/vikit/csv_utils.h
  include/aslam/common/entrypoint.h
  include/aslam/common/macros.h
  include/aslam/common/memory.h
  include/aslam/common/pose-types.h
  include/aslam/common/yaml-serialization.h
  include/aslam/common/yaml-serialization-eigen.h
  )

set(SOURCES
  src/homography.cpp
  src/math_utils.cpp
  src/performance_monitor.cpp
  src/sample.cpp
  src/user_input_thread.cpp
  src/vision.cpp
  )

# Create vikit shared library
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  cv_bridge
  minkindr
  rclcpp
)
target_link_libraries(${PROJECT_NAME}
  ${OpenCV_LIBRARIES}
  yaml-cpp
  glog::glog
)
################################################################################
# Unit tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(homography test/test_homography.cpp)
  target_link_libraries(homography ${PROJECT_NAME})

  ament_add_gtest(math_utils test/test_math_utils.cpp)
  target_link_libraries(math_utils ${PROJECT_NAME})
endif()

################################################################################
# Other tests
add_executable(vikit_test_timer test/test_timer.cpp)
target_link_libraries(vikit_test_timer ${PROJECT_NAME})

add_executable(vikit_test_common test/test_common.cpp)
target_link_libraries(vikit_test_common ${PROJECT_NAME})

################################################################################
# Install
install(TARGETS ${PROJECT_NAME} vikit_test_timer vikit_test_common
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
  minkindr
  rclcpp
  OpenCV
  yaml-cpp
  glog
  eigen3_cmake_module
  Eigen3
)

ament_package()
