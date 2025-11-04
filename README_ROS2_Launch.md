# HDVIO2 - ROS2 Launch Instructions

## Overview
HDVIO2 has been successfully migrated from ROS1 to ROS2. The package now includes a Python launch file that supports all the original functionality with ROS2 compatibility.

## Building the Package
```bash
cd /path/to/hdvio2.0
colcon build --packages-select hdvio2
source install/setup.bash
```

## Launching HDVIO2

### Basic Launch
Launch with default parameters:
```bash
ros2 launch hdvio2 hdvio2_launch.py
```

### Launch with Custom Parameters
You can customize the launch with various arguments:

```bash
ros2 launch hdvio2 hdvio2_launch.py \
    calib_file:=realsense \
    vio_param_file:=vio_mono_fisheye \
    use_dynamics:=true \
    quad_name:=parrot \
    record:=false
```

### Available Arguments

- **calib_file**: Camera calibration file name (default: `realsense`)
- **quad_name**: Quadrotor name for dynamics topic (default: `parrot`) 
- **vio_param_file**: VIO parameter file name (default: `vio_mono_fisheye`)
- **record**: Whether to record rosbag (default: `false`)
- **use_dynamics**: Whether to use dynamics prediction - requires TensorRT models (default: `false`)
- **rviz_config**: RViz configuration file to use (default: `rviz_config_vio_ros2.rviz`)

### View Available Arguments
```bash
ros2 launch hdvio2 hdvio2_launch.py --show-args
```

## Configuration Files

### Camera Calibration
Located in: `param/calib/`
- `realsense.yaml` - Default RealSense camera configuration

### VIO Parameters  
Located in: `param/`
- `vio_mono_fisheye.yaml` - Monocular fisheye configuration

### TensorRT Models
Located in: `net_models/`
- Place your trained `.engine` files here for dynamics prediction
- `thrust_net_flyingroom.engine` - Thrust prediction model
- `torque_net_flyingroom.engine` - Torque prediction model

## Topics

The node subscribes to:
- `/camera/fisheye1/image_raw` - Camera images
- `/camera/imu` - IMU data  
- `/parrot/agiros_pilot/mpc_command` - Dynamics commands (if enabled)

The node publishes:
- `/svo/backend_pose_imu` - Estimated pose
- Various visualization topics for RViz

## Migration Summary

✅ **Completed Migrations:**
- All ROS1 → ROS2 API conversions  
- TensorRT dependencies resolved
- Parameter system updated for ROS2
- Launch file converted to Python format
- Build system updated to ament_cmake
- All benchmark executables migrated

## Dependencies

### System Requirements
- ROS2 Humble
- CUDA 13.0+
- TensorRT 10.x (installed)
- YAML-cpp
- OpenCV
- Eigen3

### ROS2 Package Dependencies
- All SVO packages (migrated)
- vikit packages (migrated) 
- minkindr packages (migrated)
- Standard ROS2 packages (sensor_msgs, nav_msgs, etc.)

## RViz Configuration

### ROS2 Compatible (Default)
```bash
ros2 launch hdvio2 hdvio2_launch.py rviz_config:=rviz_config_vio_ros2.rviz
```
Uses the new ROS2-compatible configuration with proper plugin names (no "Orbit" errors).

### Legacy ROS1 Config (May show warnings)
```bash
ros2 launch hdvio2 hdvio2_launch.py rviz_config:=rviz_config_vio.rviz
```

## Troubleshooting

### RViz Plugin Errors
If you see errors like "rviz/Orbit could not be found", use the ROS2 config:
```bash
ros2 launch hdvio2 hdvio2_launch.py rviz_config:=rviz_config_vio_ros2.rviz
```

### Node Parameter Issues
The node expects ROS2-style parameters. All parameter loading has been migrated to ROS2 format.

## Notes

- **RViz Config**: A new ROS2-compatible RViz configuration eliminates plugin compatibility warnings
- **TensorRT**: Functionality is optional - set `use_dynamics:=false` if you don't have trained models  
- **Parameters**: All parameter files and calibration data are preserved from the original ROS1 version
- **Migration**: Complete ROS1→ROS2 API migration with backward compatibility for configuration files