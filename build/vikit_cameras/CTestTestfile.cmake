# CMake generated Testfile for 
# Source directory: /home/carbs/hdvio2.0/vikit/vikit_cameras
# Build directory: /home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vikit_cameras_tests "/usr/bin/python3" "-u" "/opt/ros/humble/share/ament_cmake_test/cmake/run_test.py" "/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras/test_results/vikit_cameras/vikit_cameras_tests.gtest.xml" "--package-name" "vikit_cameras" "--output-file" "/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras/ament_cmake_gtest/vikit_cameras_tests.txt" "--command" "/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras/vikit_cameras_tests" "--gtest_output=xml:/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras/test_results/vikit_cameras/vikit_cameras_tests.gtest.xml")
set_tests_properties(vikit_cameras_tests PROPERTIES  LABELS "gtest" REQUIRED_FILES "/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras/vikit_cameras_tests" TIMEOUT "60" WORKING_DIRECTORY "/home/carbs/hdvio2.0/eigen_checks/build/vikit_cameras" _BACKTRACE_TRIPLES "/opt/ros/humble/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/humble/share/ament_cmake_gtest/cmake/ament_add_gtest_test.cmake;86;ament_add_test;/opt/ros/humble/share/ament_cmake_gtest/cmake/ament_add_gtest.cmake;93;ament_add_gtest_test;/home/carbs/hdvio2.0/vikit/vikit_cameras/CMakeLists.txt;78;ament_add_gtest;/home/carbs/hdvio2.0/vikit/vikit_cameras/CMakeLists.txt;0;")
subdirs("gtest")
