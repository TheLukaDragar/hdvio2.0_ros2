cmake_minimum_required(VERSION 3.5)
project(svo_vio_common)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)

include(SvoSetup)

set(HEADERS
  include/svo/vio_common/logging.hpp
  include/svo/vio_common/matrix_operations.hpp
  include/svo/vio_common/matrix.hpp
  include/svo/vio_common/test_utils.hpp
  include/svo/vio_common/backend_types.hpp
)

set(SOURCES
  src/vio_common.cpp
  src/test_utils.cpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  svo_common
  geometry_msgs
  std_msgs
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
  svo_common
  geometry_msgs
  std_msgs
  Eigen3
)
ament_package()
