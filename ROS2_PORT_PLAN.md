# HDVIO2.0 ROS2 Port Plan

**Target:** Port from ROS1 Noetic → ROS2 Humble/Jazzy  
**Estimated Effort:** 6-8 weeks (1 developer)  
**Date Created:** November 4, 2025  
**Repository:** uzh-rpg/hdvio2.0

---

## Executive Summary

This document outlines the strategy for porting HDVIO2.0 from ROS1 Noetic to ROS2. The codebase contains 32 packages, 379 source files, with deep ROS1 integration primarily in the interface layers. Core algorithms (SVO, Ceres backend) are largely ROS-agnostic.

### Key Statistics
- **32 ROS packages** requiring migration
- **6 custom messages** (svo_msgs)
- **1 launch file** requiring Python rewrite
- **~2,500 lines** of ROS1 API usage (estimated)
- **No nodelets** (simplifies migration)

---

## Phase 1: Infrastructure & Build System (Week 1-2)

### 1.1 Update Package Manifests
**Effort:** 1-2 days

- [ ] Convert all 32 `package.xml` from format 2 → format 3
- [ ] Update buildtool dependencies: `catkin` → `ament_cmake`
- [ ] Update runtime dependencies:
  - `roscpp` → `rclcpp`
  - `roslib` → `ament_index_cpp`
  - `cv_bridge` → `cv_bridge` (ROS2 version)
  - `image_transport` → `image_transport` (ROS2 version)
  - `tf` → `tf2_ros`
  - `pcl_ros` → `pcl_ros` (ROS2 version)

**Priority packages:**
1. `svo_cmake` (build configuration)
2. `svo_msgs` (messages)
3. `svo_common` (core types)
4. `vikit_common` (utilities)
5. `hdvio2` (main interface)

### 1.2 Migrate Build System
**Effort:** 3-4 days

- [ ] Replace `catkin_simple` with `ament_cmake`
- [ ] Update all `CMakeLists.txt` files (32 files):
  - Remove `find_package(catkin_simple)`
  - Add `find_package(ament_cmake REQUIRED)`
  - Replace `catkin_simple()` with proper `ament_cmake` macros
  - Replace `cs_add_library()` → `add_library()` + `ament_target_dependencies()`
  - Replace `cs_add_executable()` → `add_executable()` + `ament_target_dependencies()`
  - Replace `cs_export()` → `ament_export_*()` and `ament_package()`
  - Update install rules

**Template for CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.8)
project(package_name)

# C++ standard
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
# ... other dependencies

# Add library/executable
add_library(${PROJECT_NAME} src/file.cpp)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  sensor_msgs
  # ... other dependencies
)

# Install
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(rclcpp sensor_msgs)

ament_package()
```

### 1.3 Handle External Dependencies
**Effort:** 2-3 days

- [ ] **eigen_catkin** → Use system Eigen3 (`find_package(Eigen3 REQUIRED)`)
- [ ] **glog_catkin** → Use system glog (`find_package(glog REQUIRED)`)
- [ ] **gflags_catkin** → Use system gflags (`find_package(gflags REQUIRED)`)
- [ ] **ceres_catkin** → Use system Ceres (`find_package(Ceres REQUIRED)`)
- [ ] Update `dependencies.yaml` → remove catkin wrappers
- [ ] Test that OpenCV, CUDA, TensorRT still link correctly

---

## Phase 2: Message Definitions (Week 2)

### 2.1 Migrate Custom Messages
**Effort:** 2-3 days

**Package:** `svo_msgs`

Messages to migrate:
1. `Command.msg`
2. `DenseInput.msg`
3. `DenseInputWithFeatures.msg`
4. `Feature.msg`
5. `Info.msg`
6. `NbvTrajectory.msg`

**Changes required:**
- [ ] Update `package.xml`:
  ```xml
  <buildtool_depend>ament_cmake</buildtool_depend>
  <buildtool_depend>rosidl_default_generators</buildtool_depend>
  <exec_depend>rosidl_default_runtime</exec_depend>
  <member_of_group>rosidl_interface_packages</member_of_group>
  ```

- [ ] Update `CMakeLists.txt`:
  ```cmake
  find_package(rosidl_default_generators REQUIRED)
  
  rosidl_generate_interfaces(${PROJECT_NAME}
    "msg/Command.msg"
    "msg/DenseInput.msg"
    "msg/DenseInputWithFeatures.msg"
    "msg/Feature.msg"
    "msg/Info.msg"
    "msg/NbvTrajectory.msg"
    DEPENDENCIES geometry_msgs sensor_msgs
  )
  ```

- [ ] Review message definitions for any deprecated types

---

## Phase 3: Core API Migration (Week 3-4)

### 3.1 Node Handle → Node Pointers
**Effort:** 4-5 days

**Files to update (~15 files):**
- `hdvio2/include/hdvio2/svo_interface.h`
- `hdvio2/include/hdvio2/visualizer.h`
- `hdvio2/include/hdvio2/svo_factory.h`
- `hdvio2/include/hdvio2/ceres_backend_factory.h`
- `hdvio2/src/svo_interface.cpp`
- `hdvio2/src/visualizer.cpp`
- `hdvio2/src/svo_node_base.cpp`
- `hdvio2/src/hdvio2_node.cpp`
- `hdvio2/src/benchmark_node.cpp`
- `hdvio2/src/benchmark_node_dynamics.cpp`
- `svo_ceres_backend/src/ceres_backend_publisher.cpp`
- `vikit/vikit_ros/include/vikit/*.h`

**Pattern changes:**
```cpp
// OLD (ROS1)
ros::NodeHandle nh_;
ros::NodeHandle pnh_;

// NEW (ROS2)
rclcpp::Node::SharedPtr node_;
// or inherit from rclcpp::Node
```

### 3.2 Publishers & Subscribers
**Effort:** 3-4 days

**Pattern changes:**
```cpp
// OLD (ROS1)
ros::Publisher pub_ = nh_.advertise<sensor_msgs::Imu>("topic", 10);
ros::Subscriber sub_ = nh_.subscribe("topic", 10, &Class::callback, this);
pub_.publish(msg);

// NEW (ROS2)
rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr pub_;
pub_ = node_->create_publisher<sensor_msgs::msg::Imu>("topic", 10);
rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_;
sub_ = node_->create_subscription<sensor_msgs::msg::Imu>(
    "topic", 10, std::bind(&Class::callback, this, std::placeholders::_1));
pub_->publish(msg);
```

**Key files:**
- All publisher/subscriber declarations in headers
- All callback functions (change signature: `const MsgPtr&` → `const Msg::SharedPtr`)

### 3.3 Time API
**Effort:** 1-2 days

**Pattern changes:**
```cpp
// OLD (ROS1)
ros::Time time = ros::Time::now();
ros::Time time = ros::Time().fromNSec(timestamp);
ros::Time time = ros::Time().fromSec(timestamp_sec);
double sec = time.toSec();
uint64_t nsec = time.toNSec();

// NEW (ROS2)
rclcpp::Time time = node_->now();
rclcpp::Time time(timestamp, RCL_ROS_TIME);  // timestamp in nanoseconds
rclcpp::Time time(static_cast<int64_t>(timestamp_sec * 1e9), RCL_ROS_TIME);
double sec = time.seconds();
int64_t nsec = time.nanoseconds();
```

**Search and replace across codebase:**
- `ros::Time` → `rclcpp::Time`
- `ros::Duration` → `rclcpp::Duration`
- `.toNSec()` → `.nanoseconds()`
- `.toSec()` → `.seconds()`

### 3.4 Parameter Server
**Effort:** 2-3 days

**Pattern changes:**
```cpp
// OLD (ROS1)
int param;
pnh_.param<int>("param_name", param, 10);  // with default
pnh_.getParam("param_name", param);         // no default
std::string val = vk::getParam<std::string>("namespace/param", "default");

// NEW (ROS2)
node_->declare_parameter("param_name", 10);  // declare with default
int param = node_->get_parameter("param_name").as_int();

// Or with descriptor for validation
auto descriptor = rcl_interfaces::msg::ParameterDescriptor();
descriptor.description = "Description of parameter";
node_->declare_parameter("param_name", rclcpp::ParameterValue(10), descriptor);
```

**Files requiring updates:**
- `hdvio2/src/svo_factory.cpp` (loads camera/VIO parameters)
- `hdvio2/src/ceres_backend_factory.cpp`
- `hdvio2/src/svo_interface.cpp`
- `vikit/vikit_ros/include/vikit/params_helper.h`
- All benchmark nodes

### 3.5 Image Transport
**Effort:** 2 days

**Pattern changes:**
```cpp
// OLD (ROS1)
#include <image_transport/image_transport.h>
image_transport::ImageTransport it(nh);
image_transport::Publisher pub = it.advertise("topic", 1);
image_transport::Subscriber sub = it.subscribe("topic", 1, callback);
image_transport::SubscriberFilter sub_filter(it, "topic", 1);

// NEW (ROS2)
#include <image_transport/image_transport.hpp>
image_transport::ImageTransport it(node_);
image_transport::Publisher pub = it.advertise("topic", 1);
image_transport::Subscriber sub = it.subscribe("topic", 1, callback);
// SubscriberFilter → use message_filters directly with image topics
```

### 3.6 TF Broadcasting
**Effort:** 1-2 days

**Pattern changes:**
```cpp
// OLD (ROS1)
#include <tf/transform_broadcaster.h>
tf::TransformBroadcaster br_;
geometry_msgs::TransformStamped tf_msg;
br_.sendTransform(tf_msg);

// NEW (ROS2)
#include <tf2_ros/transform_broadcaster.h>
std::shared_ptr<tf2_ros::TransformBroadcaster> br_;
br_ = std::make_shared<tf2_ros::TransformBroadcaster>(node_);
geometry_msgs::msg::TransformStamped tf_msg;
br_->sendTransform(tf_msg);
```

### 3.7 Message Filters & Synchronization
**Effort:** 2-3 days

**Pattern changes:**
```cpp
// OLD (ROS1)
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/synchronizer.h>

typedef message_filters::sync_policies::ExactTime<
    sensor_msgs::Image, sensor_msgs::Image> ExactPolicy;
message_filters::Synchronizer<ExactPolicy> sync_;

// NEW (ROS2)
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/synchronizer.h>

typedef message_filters::sync_policies::ExactTime<
    sensor_msgs::msg::Image, sensor_msgs::msg::Image> ExactPolicy;
message_filters::Synchronizer<ExactPolicy> sync_;
// Note: still need to adapt to ROS2 node interface
```

### 3.8 cv_bridge
**Effort:** 1 day

**Pattern changes:**
```cpp
// OLD (ROS1)
#include <cv_bridge/cv_bridge.h>
cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg);
cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, "mono8");

// NEW (ROS2)
#include <cv_bridge/cv_bridge.h>
cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(*msg);  // dereference SharedPtr
cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(*msg, "mono8");
```

### 3.9 Logging
**Effort:** 1 day

**Pattern changes:**
```cpp
// OLD (ROS1)
ROS_INFO("Message");
ROS_INFO_STREAM("Message " << var);
ROS_WARN("Warning");
ROS_ERROR("Error");
ROS_DEBUG("Debug");

// NEW (ROS2)
RCLCPP_INFO(node_->get_logger(), "Message");
RCLCPP_INFO_STREAM(node_->get_logger(), "Message " << var);
RCLCPP_WARN(node_->get_logger(), "Warning");
RCLCPP_ERROR(node_->get_logger(), "Error");
RCLCPP_DEBUG(node_->get_logger(), "Debug");
```

---

## Phase 4: Node Architecture (Week 4-5)

### 4.1 Main Nodes
**Effort:** 3-4 days

**Nodes to migrate:**
1. `hdvio2_node` (main VIO node)
2. `svo_benchmark` (benchmark without dynamics)
3. `hdvio2_benchmark` (benchmark with dynamics)

**Pattern for main():**
```cpp
// OLD (ROS1)
int main(int argc, char** argv) {
  ros::init(argc, argv, "node_name");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");
  
  MyNode node(nh, pnh);
  ros::spin();
  return 0;
}

// NEW (ROS2)
int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyNode>("node_name");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
```

**Convert to rclcpp::Node inheritance:**
```cpp
// NEW (ROS2)
class SvoInterface : public rclcpp::Node {
public:
  SvoInterface(const std::string& node_name)
    : Node(node_name)
  {
    // Initialize publishers, subscribers, parameters
    initializePublishers();
    initializeSubscribers();
    declareParameters();
  }
  
private:
  void initializePublishers() {
    pub_ = this->create_publisher<sensor_msgs::msg::Imu>("imu", 10);
  }
  
  void declareParameters() {
    this->declare_parameter("cam0_topic", "/camera/image_raw");
    cam0_topic_ = this->get_parameter("cam0_topic").as_string();
  }
};
```

### 4.2 Threading Model
**Effort:** 2-3 days

**Current:** Uses `std::thread` for IMU/dynamics/image processing

**Options:**
1. **Keep std::thread** (minimal changes, but not idiomatic ROS2)
2. **Use rclcpp executors** (recommended for ROS2)

**Recommended approach (callback groups):**
```cpp
// Create callback groups
callback_group_imu_ = this->create_callback_group(
    rclcpp::CallbackGroupType::MutuallyExclusive);
callback_group_image_ = this->create_callback_group(
    rclcpp::CallbackGroupType::MutuallyExclusive);

// Assign to subscriptions
auto sub_options = rclcpp::SubscriptionOptions();
sub_options.callback_group = callback_group_imu_;
imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
    "imu_topic", 10, callback, sub_options);

// Use MultiThreadedExecutor
auto executor = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
executor->add_node(node);
executor->spin();
```

---

## Phase 5: Launch & Configuration (Week 5)

### 5.1 Launch Files
**Effort:** 2-3 days

Convert `hdvio2.launch` (XML) → Python

**OLD (ROS1 XML):**
```xml
<launch>
  <arg name="calib_file" default="realsense"/>
  <node pkg="hdvio2" type="hdvio2_node" name="svo" args="--v=1">
    <param name="cam0_topic" value="/camera/fisheye1/image_raw"/>
    <param name="imu_topic" value="/camera/imu"/>
    <rosparam file="$(find hdvio2)/param/vio_mono_fisheye.yaml"/>
  </node>
  <node name="vis" pkg="rviz" type="rviz" args="-d $(find hdvio2)/rviz_config_vio.rviz"/>
</launch>
```

**NEW (ROS2 Python):**
```python
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_share = FindPackageShare('hdvio2')
    
    calib_file = LaunchConfiguration('calib_file')
    quad_name = LaunchConfiguration('quad_name')
    vio_param_file = LaunchConfiguration('vio_param_file')
    
    return LaunchDescription([
        DeclareLaunchArgument('calib_file', default_value='realsense'),
        DeclareLaunchArgument('quad_name', default_value='parrot'),
        DeclareLaunchArgument('vio_param_file', default_value='vio_mono_fisheye'),
        
        Node(
            package='hdvio2',
            executable='hdvio2_node',
            name='svo',
            output='screen',
            parameters=[
                PathJoinSubstitution([pkg_share, 'param', vio_param_file]),
                {
                    'cam0_topic': '/camera/fisheye1/image_raw',
                    'imu_topic': '/camera/imu',
                    'use_dynamics': True,
                    'dynamics_topic': ['/parrot/agiros_pilot/mpc_command'],
                    # ... other parameters
                }
            ],
            arguments=['--ros-args', '--log-level', 'info']
        ),
        
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', PathJoinSubstitution([pkg_share, 'rviz_config_vio.rviz'])]
        )
    ])
```

### 5.2 Parameter Files
**Effort:** 1 day

YAML parameter files mostly compatible, but review:
- [ ] Ensure all namespacing is correct
- [ ] Convert any deprecated parameters
- [ ] Test parameter loading with `ros2 param load`

---

## Phase 6: Visualization (Week 5)

### 6.1 RViz Configuration
**Effort:** 1 day

- [ ] Convert `rviz_config_vio.rviz` (ROS1) → ROS2 format
- [ ] Update display plugin names
- [ ] Test with RViz2

### 6.2 Publishers
**Effort:** 1 day

Files: `hdvio2/src/visualizer.cpp`, `svo_ceres_backend/src/ceres_backend_publisher.cpp`

- [ ] Update message types (`nav_msgs::Odometry` → `nav_msgs::msg::Odometry`)
- [ ] Update visualization markers
- [ ] Update point cloud publishing (PCL-ROS2)

---

## Phase 7: Testing & Validation (Week 6-8)

### 7.1 Unit Tests
**Effort:** 3-4 days

- [ ] Port existing tests to ROS2 testing framework
- [ ] Update test CMakeLists:
  ```cmake
  if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)
    ament_add_gtest(test_name test/test_file.cpp)
    target_link_libraries(test_name ${PROJECT_NAME})
  endif()
  ```
- [ ] Run all tests: `colcon test --packages-select <package_name>`

### 7.2 Integration Testing
**Effort:** 3-4 days

- [ ] Test with ROS2 bag files (convert ROS1 bags if needed)
- [ ] Verify all topics are publishing
- [ ] Check TF tree
- [ ] Monitor performance (latency, CPU usage)
- [ ] Test multi-threading behavior

### 7.3 Bag File Conversion
**Effort:** 1 day

```bash
# Convert ROS1 bag to ROS2
ros2 bag convert --input flyingroom_flight.bag --output-options output.yaml
```

### 7.4 Benchmark Against ROS1
**Effort:** 2-3 days

Compare:
- [ ] Trajectory accuracy
- [ ] Latency
- [ ] CPU/memory usage
- [ ] Dynamics estimation quality

---

## Phase 8: Documentation & Cleanup (Week 8)

### 8.1 Update Documentation
**Effort:** 2 days

- [ ] Update README.md with ROS2 installation instructions
- [ ] Update dependencies (remove ROS1 references)
- [ ] Document ROS2-specific quirks
- [ ] Update launch file documentation
- [ ] Create migration guide for users

### 8.2 CI/CD
**Effort:** 1-2 days

- [ ] Update CI to build with ROS2
- [ ] Add ROS2 Humble/Jazzy workflows
- [ ] Test on Ubuntu 22.04/24.04

---

## Migration Order (Dependency-Based)

### Stage 1: Foundation (Week 1-2)
1. `svo_cmake` - Build configuration
2. `eigen_catkin` → Remove (use system Eigen)
3. `glog_catkin` → Remove (use system glog)
4. `gflags_catkin` → Remove (use system gflags)
5. `catkin_simple` → Remove (use ament_cmake)
6. `svo_msgs` - Custom messages
7. `minkindr` - Math utilities (minimal ROS)
8. `eigen_checks` - Utilities

### Stage 2: Core Libraries (Week 2-3)
9. `vikit_common` - Vision toolkit
10. `vikit_cameras` - Camera models
11. `vikit_solver` - Optimization utilities
12. `svo_common` - SVO common types
13. `fast_neon` - FAST corner detector
14. `rpg_common` - RPG utilities

### Stage 3: SVO Components (Week 3-4)
15. `opengv` - Geometric vision
16. `svo_img_align` - Image alignment
17. `svo_direct` - Direct method
18. `svo_tracker` - Feature tracking
19. `svo_vio_common` - VIO common
20. `svo_test_utils` - Test utilities
21. `svo` - Main SVO library

### Stage 4: Backend & Interface (Week 4-5)
22. `ceres_catkin` → Remove (use system Ceres)
23. `svo_ceres_backend` - Backend optimizer
24. `vikit_ros` - ROS interface utilities
25. `minkindr_ros` - ROS conversions
26. `hdvio2` - Main interface & nodes

### Stage 5: Tools & Testing (Week 5-6)
27. `svo_benchmarking` - Benchmarking tools
28. Launch files & configuration
29. RViz configuration
30. Integration testing

---

## Risk Mitigation

### High Risk Items
1. **Message filter synchronization** - API changes significantly
   - Mitigation: Test early with stereo callback
   
2. **Multi-threading with ROS2 executors** - Different paradigm
   - Mitigation: Consider keeping std::thread initially, migrate later
   
3. **CUDA/TensorRT compatibility** - May need recompilation
   - Mitigation: Test on target platform early

4. **Performance regression** - ROS2 overhead
   - Mitigation: Benchmark early and often

### Medium Risk Items
1. **Parameter loading** - Different YAML structure
   - Mitigation: Thorough testing with existing configs
   
2. **TF2 migration** - More complex than TF
   - Mitigation: Use tf2_ros wrapper functions

3. **Image transport** - API differences
   - Mitigation: Reference ROS2 image_transport examples

---

## Testing Strategy

### 1. Incremental Testing
- Build and test each package after migration
- Use ROS2 bag replay for consistent testing

### 2. Regression Testing
- Compare against ROS1 version using same dataset
- Metrics: trajectory RMSE, timing, CPU usage

### 3. Integration Testing
- Full pipeline test with rosbag
- Test all launch configurations
- Multi-camera support

### 4. Performance Testing
- Latency profiling
- Memory usage
- CPU utilization
- Real-time factor

---

## Tooling & Automation

### Recommended Tools
1. **ros1_bridge** - Run ROS1 and ROS2 side-by-side during migration
2. **colcon** - Build system
3. **rosbag2** - Bag file recording/playback
4. **ros2_tracing** - Performance analysis

### Scripts to Create
```bash
# scripts/migrate_package.sh - Template for package migration
# scripts/check_ros1_api.sh - Find remaining ROS1 API usage
# scripts/test_all.sh - Run all tests
# scripts/compare_output.sh - Compare ROS1 vs ROS2 output
```

---

## Success Criteria

- [ ] All 32 packages build successfully with colcon
- [ ] All unit tests pass
- [ ] Main node runs without errors
- [ ] Publishes all expected topics
- [ ] TF tree is correct
- [ ] Trajectory accuracy within 5% of ROS1 version
- [ ] Latency within 10% of ROS1 version
- [ ] Successfully processes benchmark rosbag
- [ ] Documentation updated
- [ ] CI/CD pipeline working

---

## Target Platform

**Recommended:** Ubuntu 22.04 LTS + ROS2 Humble
- Long-term support until 2027
- Stable CUDA support (11.x/12.x)
- TensorRT 8.x compatibility
- Well-tested ecosystem

**Alternative:** Ubuntu 24.04 LTS + ROS2 Jazzy
- Newer platform
- ROS2 Jazzy support until 2029
- May have newer library versions

---

## Timeline Summary

| Week | Phase | Deliverables |
|------|-------|--------------|
| 1 | Infrastructure | package.xml updated, build system migrated |
| 2 | Messages & Foundation | svo_msgs working, core libraries building |
| 3 | API Migration Part 1 | Publishers/subscribers/time API converted |
| 4 | API Migration Part 2 | Parameters, image_transport, TF2 working |
| 5 | Nodes & Launch | Main nodes running, launch files working |
| 6 | Testing Part 1 | Unit tests passing, basic integration tests |
| 7 | Testing Part 2 | Full integration tests, benchmarking |
| 8 | Polish | Documentation, CI/CD, final validation |

---

## Notes

- Keep ROS1 version in separate branch for comparison
- Consider maintaining both versions temporarily
- Document all breaking changes
- Create migration guide for downstream users
- Test on actual robot hardware before declaring complete

---

## Next Steps

1. ✅ Review and approve this plan
2. Set up ROS2 development environment
3. Create `ros2_humble` branch
4. Begin with Stage 1 packages
5. Set up CI/CD for ROS2 builds

