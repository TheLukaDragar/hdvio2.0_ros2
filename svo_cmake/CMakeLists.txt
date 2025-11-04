cmake_minimum_required(VERSION 3.8)
project(svo_cmake)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)

# Install all included cmake files
install(DIRECTORY cmake/Modules
  DESTINATION share/${PROJECT_NAME}/cmake
)

ament_package(
  CONFIG_EXTRAS cmake/svo_cmake-extras.cmake
)
