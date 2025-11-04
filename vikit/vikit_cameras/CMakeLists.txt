cmake_minimum_required(VERSION 3.5)
project(vikit_cameras)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(eigen_checks REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(minkindr REQUIRED)
find_package(vikit_common REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(glog REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)

include(SvoSetup)

set(HEADERS
  include/vikit/cameras.h
  include/vikit/cameras/camera_geometry_base.h
  include/vikit/cameras/implementation/camera_geometry_base.hpp
  include/vikit/cameras/no_distortion.h
  include/vikit/cameras/atan_distortion.h
  include/vikit/cameras/equidistant_distortion.h
  include/vikit/cameras/equidistant_fisheye_geometry.h
  include/vikit/cameras/equidistant_fisheye_projection.h
  include/vikit/cameras/omni_geometry.h
  include/vikit/cameras/omni_projection.h
  include/vikit/cameras/radial_tangential_distortion.h
  include/vikit/cameras/pinhole_projection.h
  include/vikit/cameras/implementation/pinhole_projection.hpp
  include/vikit/cameras/camera_geometry.h
  include/vikit/cameras/implementation/camera_geometry.hpp
  include/vikit/cameras/camera_factory.h
  include/vikit/cameras/ncamera.h
  include/vikit/cameras/yaml/camera-yaml-serialization.h
  include/vikit/cameras/yaml/ncamera-yaml-serialization.h
  )

set(SOURCES
  src/camera_factory.cpp
  src/ncamera.cpp
  src/camera_geometry_base.cpp
  src/camera_yaml_serialization.cpp
  src/equidistant_fisheye_geometry.cpp
  src/equidistant_fisheye_projection.cpp
  src/omni_geometry.cpp
  src/omni_projection.cpp
  src/ncamera_yaml_serialization.cpp
  )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME}
  eigen_checks
  cv_bridge
  minkindr
  vikit_common
)
target_link_libraries(${PROJECT_NAME}
  yaml-cpp
  glog::glog
)

################################################################################
# Tests

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(${PROJECT_NAME}_tests
    test/test_cameras
  )
  target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})
endif()

################################################################################
# Install

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
  eigen_checks
  cv_bridge
  minkindr
  vikit_common
  yaml-cpp
  glog
  eigen3_cmake_module
  Eigen3
)

ament_package()
