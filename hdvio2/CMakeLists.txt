cmake_minimum_required(VERSION 3.5)
project(hdvio2)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(minkindr REQUIRED)
find_package(minkindr_conversions REQUIRED)
find_package(rpg_common REQUIRED)
find_package(eigen_checks REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(svo REQUIRED)
find_package(svo_cmake REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_msgs REQUIRED)
find_package(svo_tracker REQUIRED)
find_package(svo_ceres_backend REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(CUDA REQUIRED)

set(TRT_ROOT $ENV{TRT_ROOT})
if(NOT TRT_ROOT)
  set(TRT_ROOT "/usr")
endif()

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

include_directories(
  include
  ${TRT_ROOT}/include
  ${CUDA_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} SHARED
  ${SOURCES}
  ${HEADERS}
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_components
  cv_bridge
  image_transport
  sensor_msgs
  std_msgs
  nav_msgs
  visualization_msgs
  tf2
  tf2_ros
  pcl_conversions
  minkindr
  minkindr_conversions
  rpg_common
  eigen_checks
  vikit_common
  vikit_ros
  svo
  svo_common
  svo_msgs
  svo_tracker
  svo_ceres_backend
)

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

add_executable(svo_benchmark src/benchmark_node.cpp)
target_link_libraries(svo_benchmark ${PROJECT_NAME})
ament_target_dependencies(svo_benchmark rclcpp)

add_executable(hdvio2_node src/hdvio2_node.cpp)
target_link_libraries(hdvio2_node ${PROJECT_NAME})
ament_target_dependencies(hdvio2_node rclcpp)

add_executable(hdvio2_benchmark src/benchmark_node_dynamics.cpp)
target_link_libraries(hdvio2_benchmark ${PROJECT_NAME})
ament_target_dependencies(hdvio2_benchmark rclcpp)

install(TARGETS
  ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS
  svo_benchmark
  hdvio2_node
  hdvio2_benchmark
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY launch param
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  rclcpp
  cv_bridge
  sensor_msgs
  minkindr
  svo
  svo_ceres_backend
)

ament_package()