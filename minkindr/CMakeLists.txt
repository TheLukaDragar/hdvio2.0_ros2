cmake_minimum_required(VERSION 3.8)
project(minkindr)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(eigen_checks REQUIRED)
find_package(Threads REQUIRED)
find_package(glog REQUIRED)

# Header-only library
include_directories(include)

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  
  ament_add_gtest(${PROJECT_NAME}_tests
    test/test-main.cc
    test/test-rotation.cc
    test/test-sim3.cc
    test/test-transformation.cc
    test/test-transformation-2d.cc
  )
  target_include_directories(${PROJECT_NAME}_tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  ament_target_dependencies(${PROJECT_NAME}_tests
    Eigen3
    eigen_checks
  )
  target_link_libraries(${PROJECT_NAME}_tests Threads::Threads glog::glog)
endif()

# Install
install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_dependencies(Eigen3 eigen_checks)

ament_package()

