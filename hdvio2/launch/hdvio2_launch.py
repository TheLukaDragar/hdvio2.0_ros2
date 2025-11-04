#!/usr/bin/env python3

import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Launch arguments (same as ROS1)
    calib_file_arg = DeclareLaunchArgument('calib_file', default_value='realsense')
    quad_name_arg = DeclareLaunchArgument('quad_name', default_value='parrot')
    vio_param_file_arg = DeclareLaunchArgument('vio_param_file', default_value='vio_mono_fisheye')
    record_arg = DeclareLaunchArgument('record', default_value='false')

    # Get arguments
    calib_file = LaunchConfiguration('calib_file')
    quad_name = LaunchConfiguration('quad_name')
    
    # Package path
    pkg_path = get_package_share_directory('hdvio2')

    # SVO node (direct port from ROS1)
    svo_node = Node(
        package='hdvio2',
        executable='hdvio2_node',
        name='svo',
        output='screen',
        parameters=[
            {
                'cam0_topic': '/camera/fisheye1/image_raw',
                'imu_topic': '/camera/imu',
                'use_dynamics': True,
                'dynamics_topic': '/parrot/agiros_pilot/mpc_command',
                'external_force_zero_mean': False,
                'use_learned_residuals': True,
                'use_rotational_dynamics': True,
                'thrust_net_model_file': os.path.join(pkg_path, 'net_models', 'thrust_net_flyingroom.engine'),
                'torque_net_model_file': os.path.join(pkg_path, 'net_models', 'torque_net_flyingroom.engine'),
                'calib_file': os.path.join(pkg_path, 'param', 'calib', 'realsense.yaml'),
            },
            os.path.join(pkg_path, 'param', 'vio_mono_fisheye_ros2.yaml'),
        ]
    )

    # RViz node (direct port from ROS1)
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='vis',
        arguments=['-d', os.path.join(pkg_path, 'rviz_config_vio_ros2.rviz')]
    )

    return LaunchDescription([
        calib_file_arg,
        quad_name_arg,
        vio_param_file_arg,
        record_arg,
        svo_node,
        rviz_node
    ])