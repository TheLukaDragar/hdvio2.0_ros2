cmake_minimum_required(VERSION 3.5)
project(rpg_common)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(eigen_checks REQUIRED)
find_package(minkindr REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)

FILE(GLOB_RECURSE HEADERS "include/*")
list(APPEND SOURCES src/fs.cpp)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)

ament_target_dependencies(${PROJECT_NAME}
  cv_bridge
  eigen_checks
  minkindr
  OpenCV
)

target_link_libraries(${PROJECT_NAME}
  glog::glog
  gflags
)

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
  cv_bridge
  eigen_checks
  minkindr
  OpenCV
  Eigen3
)
ament_package()
