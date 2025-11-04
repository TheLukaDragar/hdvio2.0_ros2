cmake_minimum_required(VERSION 3.5)
project(svo)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_direct REQUIRED)
find_package(svo_tracker REQUIRED)
find_package(svo_test_utils REQUIRED)
find_package(svo_img_align REQUIRED)
find_package(svo_vio_common REQUIRED)
find_package(rpg_common REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(vikit_solver REQUIRED)
find_package(minkindr REQUIRED)
find_package(fast_neon REQUIRED)
find_package(opengv REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)

include(SvoSetup)

################################################################################
# Cuda related
find_package(CUDA REQUIRED)
message("CUDA Libs: ${CUDA_LIBRARIES}")
message("CUDA Headers: ${CUDA_INCLUDE_DIRS}")

# CuDNN related
option(CUDNN_AVAIL "CUDNN available" OFF)
find_library(CUDNN_LIBRARY
        NAMES libcudnn.so
        PATHS /usr/local/cuda/lib64
        )
if (CUDNN_LIBRARY)
    message("CUDNN is available!")
    message("CUDNN_LIBRARY: ${CUDNN_LIBRARY}")
    set(CUDNN_AVAIL ON)
else ()
    message("CUDNN is NOT Available")
    set(CUDNN_AVAIL OFF)
endif ()

# TensorRT related
set(TRT_ROOT $ENV{TRT_ROOT})
find_library(NVINFER NAMES nvinfer HINTS ${TRT_ROOT}/lib)
find_library(NVPARSERS NAMES nvparsers HINTS ${TRT_ROOT}/lib)
find_library(NVINFERPLUGIN NAMES nvinfer_plugin HINTS ${TRT_ROOT}/lib)
find_library(NVONNXPARSER NAMES nvonnxparser HINTS ${TRT_ROOT}/lib)

if (NVINFER AND NVPARSERS AND NVINFERPLUGIN AND NVONNXPARSER)
    message("TensorRT is available!")
    message("NVINFER: ${NVINFER}")
    message("NVPARSERS: ${NVPARSERS}")
    message("NVINFERPLUGIN: ${NVINFERPLUGIN}")
    message("NVONNXPARSER: ${NVONNXPARSER}")
    set(TRT_AVAIL ON)
else ()
    message("TensorRT is NOT Available")
    set(TRT_AVAIL OFF)
endif ()

# CUDA related compilation
SET(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
SET(CUDA_NVCC_FLAGS "-std=c++11 -arch=sm_52 -lineinfo -Xcompiler '-fPIC'" CACHE STRING "nvcc flags" FORCE)
SET(CUDA_VERBOSE_BUILD ON CACHE BOOL "nvcc verbose" FORCE)

################################################################################

list(APPEND SVO_SOURCEFILES
  src/frame_handler_mono.cpp
  src/frame_handler_stereo.cpp
  src/frame_handler_array.cpp
  src/frame_handler_base.cpp
  src/map.cpp
  src/pose_optimizer.cpp
  src/initialization.cpp
  src/reprojector.cpp
  src/imu_handler.cpp
  src/dynamics_handler.cpp
  src/dynamics_net.cpp
  src/quad_angvel_spline.cpp
  src/opt_quad_angvel_spline.cpp
  src/stereo_triangulation.cpp
  src/io.cpp
)

list(APPEND SVO_HEADERFILES
  include/svo/abstract_bundle_adjustment.h
  include/svo/frame_handler_base.h
  include/svo/frame_handler_mono.h
  include/svo/frame_handler_stereo.h
  include/svo/frame_handler_array.h
  include/svo/global.h
  include/svo/imu_handler.h
  include/svo/dynamics_handler.h
  include/svo/dynamics_net.h
  include/svo/quad_angvel_spline.h
  include/svo/opt_quad_angvel_spline.h
  include/svo/initialization.h
  include/svo/map.h
  include/svo/pose_optimizer.h
  include/svo/reprojector.h
  include/svo/stereo_triangulation.h
  include/svo/io.h
  include/svo/svo.h
  )

################################################################################
# Create svo library
add_library(${PROJECT_NAME} SHARED ${SVO_SOURCEFILES} ${SVO_HEADERFILES})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
  ${TRT_ROOT}/include
  ${CUDA_INCLUDE_DIRS}
)

ament_target_dependencies(${PROJECT_NAME}
  svo_common
  svo_direct
  svo_tracker
  svo_test_utils
  svo_img_align
  svo_vio_common
  rpg_common
  vikit_common
  vikit_cameras
  vikit_solver
  minkindr
  fast_neon
  opengv
  cv_bridge
)

target_link_libraries(${PROJECT_NAME}
  glog::glog
  ${NVINFER}
  ${NVPARSERS}
  ${NVINFERPLUGIN}
  ${CUDA_LIBRARIES}
  ${CUDA_CUBLAS_LIBRARIES}
  ${CUDA_curand_LIBRARY}
  ${CUDNN_LIBRARY}
  ${NVONNXPARSER}
  ${TensorRT_LIBRARIES}
)

################################################################################
###
### GTEST
###

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(${PROJECT_NAME}-test-frame test/test_frame.cpp)
  target_link_libraries(${PROJECT_NAME}-test-frame ${PROJECT_NAME})
endif()

################################################################################
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
  svo_tracker
  svo_img_align
  svo_vio_common
  rpg_common
  vikit_common
  vikit_cameras
  minkindr
  fast_neon
  opengv
  cv_bridge
  Eigen3
)
ament_package()
