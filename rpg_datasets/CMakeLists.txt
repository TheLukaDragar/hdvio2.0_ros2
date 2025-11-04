cmake_minimum_required(VERSION 3.5)
project(rpg_datasets)

find_package(ament_cmake REQUIRED)

# Install dataset directories
install(DIRECTORY
  rpg_vfr_pinhole/
  DESTINATION share/${PROJECT_NAME}/rpg_vfr_pinhole
)

install(DIRECTORY
  rpg_urban_pinhole/
  DESTINATION share/${PROJECT_NAME}/rpg_urban_pinhole
)

ament_package()
