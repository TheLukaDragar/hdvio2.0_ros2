cmake_minimum_required(VERSION 3.5)
project(minkindr_conversions)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(glog REQUIRED)
find_package(minkindr REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(Eigen3 REQUIRED)

# Header-only library
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)

ament_target_dependencies(${PROJECT_NAME} INTERFACE
  geometry_msgs
  minkindr
  tf2
  tf2_eigen
)

target_link_libraries(${PROJECT_NAME} INTERFACE
  glog::glog
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(tf2_geometry_msgs REQUIRED)
  
  ament_add_gtest(kindr_tf_test test/kindr_tf_test.cc)
  target_link_libraries(kindr_tf_test ${PROJECT_NAME})
  ament_target_dependencies(kindr_tf_test geometry_msgs minkindr tf2 tf2_eigen tf2_geometry_msgs)
  
  ament_add_gtest(kindr_msg_test test/kindr_msg_test.cc)
  target_link_libraries(kindr_msg_test ${PROJECT_NAME})
  ament_target_dependencies(kindr_msg_test geometry_msgs minkindr tf2_eigen)
endif()

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)
ament_export_dependencies(
  geometry_msgs
  minkindr
  tf2
  tf2_eigen
  glog
)

ament_package()
