cmake_minimum_required(VERSION 3.5)
project(svo_direct)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_test_utils REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(vikit_solver REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)
find_package(yaml-cpp REQUIRED)

include(SvoSetup)

set(HEADERS
  include/svo/direct/patch_warp.h
  include/svo/direct/patch_score.h
  include/svo/direct/patch_utils.h
  include/svo/direct/feature_alignment.h
  include/svo/direct/feature_detection_types.h
  include/svo/direct/feature_detection.h
  include/svo/direct/feature_detection_utils.h
  include/svo/direct/matcher.h
  include/svo/direct/depth_filter.h
  include/svo/direct/depth_estimation.h
  include/svo/direct/elder_zucker.h
)

set(SOURCES
  src/patch_warp.cpp
  src/feature_alignment.cpp
  src/feature_detection.cpp
  src/feature_detection_utils.cpp
  src/matcher.cpp
  src/depth_filter.cpp
  src/depth_estimation.cpp
  src/elder_zucker.cpp
  )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  svo_common
  svo_test_utils
  vikit_ros
  vikit_solver
  cv_bridge
  ament_index_cpp
)
target_link_libraries(${PROJECT_NAME}
  ${OpenCV_LIBRARIES}
  glog::glog
  gflags
  yaml-cpp
)

add_executable(svo_test_patch_score test/test_patch_score.cpp)
ament_target_dependencies(svo_test_patch_score svo_common svo_test_utils)
target_link_libraries(svo_test_patch_score ${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp)

add_executable(svo_test_edgelet_direction test/test_edgelet_direction.cpp)
ament_target_dependencies(svo_test_edgelet_direction svo_common svo_test_utils)
target_link_libraries(svo_test_edgelet_direction ${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp)

add_executable(svo_test_edgelet_extraction test/test_edgelet_extraction.cpp)
ament_target_dependencies(svo_test_edgelet_extraction svo_common svo_test_utils)
target_link_libraries(svo_test_edgelet_extraction ${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp)

add_executable(svo_test_feature_alignment test/test_feature_alignment.cpp)
ament_target_dependencies(svo_test_feature_alignment svo_common svo_test_utils)
target_link_libraries(svo_test_feature_alignment ${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp)

################################################################################
###
### GTEST
###
# Test disabled - has linking issues with OpenCV in vikit_cameras
# if(BUILD_TESTING)
#   find_package(ament_cmake_gtest REQUIRED)
#   ament_add_gtest(${PROJECT_NAME}-test-depth-estimation
#     test/test_depth_estimation.cpp
#     test/test_utils.h
#   )
#   target_link_libraries(${PROJECT_NAME}-test-depth-estimation ${PROJECT_NAME} pthread yaml-cpp)
#   ament_target_dependencies(${PROJECT_NAME}-test-depth-estimation svo_common)
# endif()

################################################################################
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS
  svo_test_patch_score
  svo_test_edgelet_direction
  svo_test_edgelet_extraction
  svo_test_feature_alignment
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  svo_common
  svo_test_utils
  vikit_ros
  vikit_solver
  cv_bridge
  ament_index_cpp
  OpenCV
  Eigen3
)
ament_package()

