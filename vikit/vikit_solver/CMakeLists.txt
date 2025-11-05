cmake_minimum_required(VERSION 3.5)
project(vikit_solver)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ament_cmake REQUIRED)
find_package(glog REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)

#############
# LIBRARIES #
#############
add_library(${PROJECT_NAME}
  include/vikit/solver/mini_least_squares_solver.h
  include/vikit/solver/implementation/mini_least_squares_solver.hpp
  src/robust_cost.cpp
  include/vikit/solver/robust_cost.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
  glog::glog
)

## Declare a cpp executable
#add_executable(elevation_map_node src/elevation_map_node.cpp)
#target_link_libraries(elevation_map_node ${PROJECT_NAME})

##########
# GTESTS #
##########
#if(BUILD_TESTING)
#  find_package(ament_cmake_gtest REQUIRED)
#  ament_add_gtest(test_cameras test/test-cameras.cc)
#  target_link_libraries(test_cameras ${PROJECT_NAME})
#endif()

##########
# EXPORT #
##########
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
  glog
  eigen3_cmake_module
  Eigen3
)

ament_package()
