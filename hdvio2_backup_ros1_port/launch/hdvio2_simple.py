#!/usr/bin/env python3

import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Get package share directory
    hdvio2_share_path = get_package_share_directory('hdvio2')

    # SVO Node with minimal working configuration
    svo_node = Node(
        package='hdvio2',
        executable='hdvio2_node', 
        name='svo',
        output='screen',
        parameters=[{
            'calib_file': os.path.join(hdvio2_share_path, 'param', 'calib', 'realsense.yaml'),
            'cam0_topic': '/camera/fisheye1/image_raw',
            'imu_topic': '/camera/imu',
            'use_dynamics': False,
            # Add basic SVO parameters inline
            'automatic_reinitialization': True,
            'pipeline_is_stereo': False,
            'max_fts': 180,
            'max_n_kfs': 10,
            'map_scale': 5.0,
            'grid_size': 25,
            'n_pyr_levels': 3,
            'detector_threshold_primary': 10.0,
            'img_align_max_level': 4,
            'img_align_min_level': 2,
            'poseoptim_thresh': 2.0,
            'seed_convergence_sigma2_thresh': 200.0,
            'publish_marker_scale': 0.5,
            'ceres_verbose': False,
            'ceres_num_iterations': 5,
        }]
    )

    # RViz Node
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz',
        arguments=['-d', os.path.join(hdvio2_share_path, 'rviz_config_vio_ros2.rviz')],
        output='screen'
    )

    return LaunchDescription([
        svo_node,
        rviz_node
    ])