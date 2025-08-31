project(hdvio2)
cmake_minimum_required(VERSION 2.8.3)

find_package(catkin_simple REQUIRED)
find_package(rostest REQUIRED)
catkin_simple()
find_package(OpenCV REQUIRED)

find_package(CUDA REQUIRED)

set(TRT_ROOT $ENV{TRT_ROOT})

include(SvoSetup)

set(HEADERS
  include/hdvio2/svo_interface.h
  include/hdvio2/svo_factory.h
  include/hdvio2/visualizer.h
  include/hdvio2/csv_dataset_reader.h
  include/hdvio2/txt_reader.h
  include/hdvio2/ceres_backend_factory.h
)

set(SOURCES
  src/ceres_backend_factory.cpp
  src/svo_interface.cpp
  src/svo_factory.cpp
  src/svo_node_base.cpp
  src/visualizer.cpp
  src/csv_dataset_reader.cpp
  src/txt_reader.cpp
)

include_directories(${PROJECT_NAME} ${TRT_ROOT}/include ${CUDA_INCLUDE_DIRS})
cs_add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} 
      ${OpenCV_LIBRARIES}
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

cs_add_executable(svo_benchmark src/benchmark_node.cpp)
target_link_libraries(svo_benchmark ${PROJECT_NAME})

cs_add_executable(hdvio2_node src/hdvio2_node.cpp)
target_link_libraries(hdvio2_node ${PROJECT_NAME})

cs_add_executable(hdvio2_benchmark src/benchmark_node_dynamics.cpp)
target_link_libraries(hdvio2_benchmark ${PROJECT_NAME})

cs_export()