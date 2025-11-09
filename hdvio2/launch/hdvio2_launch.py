#!/usr/bin/env python3

import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Launch arguments
    calib_file_arg = DeclareLaunchArgument('calib_file', default_value='camera_calib8_e')
    quad_name_arg = DeclareLaunchArgument('quad_name', default_value='parrot')
    vio_param_file_arg = DeclareLaunchArgument('vio_param_file', default_value='vio_calib8e')
    use_dynamics_arg = DeclareLaunchArgument('use_dynamics', default_value='false')
    record_arg = DeclareLaunchArgument('record', default_value='false')
    
    # Set environment variables for debug logging
    # Set environment variables for debug logging
    glog_verbose = SetEnvironmentVariable('GLOG_v', '3')  # Maximum verbosity to see state transitions
    glog_stderr = SetEnvironmentVariable('GLOG_logtostderr', '1')

    # Get arguments
    calib_file = LaunchConfiguration('calib_file')
    quad_name = LaunchConfiguration('quad_name')
    vio_param_file = LaunchConfiguration('vio_param_file')
    use_dynamics = LaunchConfiguration('use_dynamics')
    
    # Package path
    pkg_path = get_package_share_directory('hdvio2')

    # SVO node with environment variable for compressed transport
    import os
    env_vars = os.environ.copy()
    env_vars['IMAGE_TRANSPORT'] = 'compressed'
    
     # SVO node with environment variable for compressed transport
    import os
    env_vars = os.environ.copy()
    env_vars['IMAGE_TRANSPORT'] = 'compressed'
    
    svo_node = Node(
        package='hdvio2',
        executable='hdvio2_node',
        name='svo',
        output='screen',
        parameters=[
            [pkg_path, '/param/', vio_param_file, '.yaml'],
            {
                'use_sim_time': False,
                'cam0_topic': '/image_raw',
                'imu_topic': '/px4_0/imu',
            },
        ],
        remappings=[
            ('/image_raw/compressed', '/image_raw/compressed'),
        ],
        additional_env=env_vars,
    )

    # # RViz node (direct port from ROS1)
    # rviz_node = Node(
    #     package='rviz2',
    #     executable='rviz2',
    #     name='vis',
    #     parameters=[{'use_sim_time': True}],  # Enable simulation time for bag playback
    #     arguments=['-d', os.path.join(pkg_path, 'rviz_config_vio_ros2.rviz')]
    # )

    return LaunchDescription([
        glog_verbose,
        glog_stderr,
        calib_file_arg,
        quad_name_arg,
        vio_param_file_arg,
        use_dynamics_arg,
        record_arg,
        svo_node,
        # rviz_node
    ])