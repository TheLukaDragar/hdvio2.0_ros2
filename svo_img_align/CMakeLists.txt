cmake_minimum_required(VERSION 3.5)
project(svo_img_align)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_direct REQUIRED)
find_package(vikit_solver REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(eigen_checks REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)

include(SvoSetup)

set(HEADERS_CPU
  include/svo/img_align/sparse_img_align_base.h
  include/svo/img_align/sparse_img_align.h
)

set(SOURCES_CPU
  src/sparse_img_align_base.cpp
  src/sparse_img_align.cpp
)

add_library(${PROJECT_NAME} ${SOURCES_CPU} ${HEADERS_CPU})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  svo_common
  svo_direct
  vikit_solver
  vikit_ros
  eigen_checks
)
target_link_libraries(${PROJECT_NAME}
  glog::glog
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
  svo_direct
  vikit_solver
  vikit_ros
  eigen_checks
  Eigen3
)
ament_package()
