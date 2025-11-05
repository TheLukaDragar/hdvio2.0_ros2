// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).

#pragma once

#include <utility>  // std::pair
#include <iostream>

#include <boost/shared_ptr.hpp>

// ros
#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <std_msgs/msg/color_rgba.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <image_transport/image_transport.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <svo_msgs/msg/info.hpp>
#include <svo_msgs/msg/dense_input_with_features.hpp>
#include <svo/global.h>
#include <svo/common/types.h>


namespace svo
{
// forward declarations
class FrameHandlerBase;

/// Publish visualisation messages to ROS.
class Visualizer
{
public:
  typedef std::shared_ptr<Visualizer> Ptr;
  typedef pcl::PointXYZI PointType;
  typedef pcl::PointCloud<PointType> PointCloud;

  static std::string kWorldFrame;

  static constexpr double seed_marker_scale_ = 0.03;
  static constexpr double seed_uncertainty_marker_scale_ = 0.03;
  static constexpr double trajectory_marker_scale_ = 0.03;
  static constexpr double point_marker_scale_ = 0.05;

  rclcpp::Node::SharedPtr pnh_;
  size_t trace_id_ = 0;
  std::string trace_dir_;
  size_t img_pub_level_;
  size_t img_pub_nth_;
  size_t dense_pub_nth_;
  bool viz_caption_str_;

  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr pub_frames_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr pub_points_;
  rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr pub_imu_pose_;
  rclcpp::Publisher<svo_msgs::msg::Info>::SharedPtr pub_info_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr pub_markers_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_pc_;
  PointCloud::Ptr pc_;
  rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr pub_path_;
  nav_msgs::msg::Path path_msg_;
  std::vector<rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr> pub_cam_poses_;
  std::vector<rclcpp::Publisher<svo_msgs::msg::DenseInputWithFeatures>::SharedPtr> pub_dense_;
  std::vector<image_transport::Publisher> pub_images_;

  std::shared_ptr<tf2_ros::TransformBroadcaster> br_;
  bool publish_world_in_cam_frame_;
  bool publish_map_every_frame_;
  rclcpp::Duration publish_points_display_time_;
  bool publish_seeds_;
  bool publish_seeds_uncertainty_;
  bool publish_active_keyframes_;
  bool trace_pointcloud_;
  double vis_scale_;
  std::ofstream ofs_pointcloud_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr pub_visible_fixed_landmarks_;
  
  std::string img_caption_;

  Visualizer(const std::string& trace_dir, const rclcpp::Node::SharedPtr& nh_private,
             const size_t num_cameras);

  ~Visualizer() = default;

  void publishSvoInfo(const svo::FrameHandlerBase* const svo,
                      const int64_t timestamp_nanoseconds);

  void publishImages(const std::vector<cv::Mat>& images,
                     const int64_t timestamp_nanoseconds);

  void publishImagesWithFeatures(const FrameBundlePtr& frame_bundle,
                                 const int64_t timestamp,
                                 const bool draw_boundary);

  void publishImuPose(const Transformation& T_world_imu,
                      const Eigen::Matrix<double, 6, 6> Covariance,
                      const int64_t timestamp_nanoseconds);

  void publishCameraPoses(const FrameBundlePtr& frame_bundle,
                          const int64_t timestamp_nanoseconds);

  void publishBundleFeatureTracks(const FrameBundlePtr frames_ref,
                                  const FrameBundlePtr frames_cur,
                                  int64_t timestamp);

  void publishFeatureTracks(
      const Keypoints& px_ref, const Keypoints& px_cur,
      const std::vector<std::pair<size_t, size_t>>& matches_ref_cur,
      const ImgPyr& img_pyr, const Level& level, const uint64_t timestamp,
      const size_t frame_index);

  void visualizeHexacopter(const Transformation& T_frame_world,
                           const uint64_t timestamp);

  void visualizeQuadrocopter(const Transformation& T_frame_world,
                             const uint64_t timestamp);

  void visualizeMarkers(const FrameBundlePtr& frame_bundle,
                        const std::vector<FramePtr>& close_kfs,
                        const MapPtr& map);

  void publishTrajectoryPoint(const Eigen::Vector3d& pos_in_vision,
                              const uint64_t timestamp, const int id);

  void visualizeMarkersWithUncertainty(const FramePtr& frame,
                                       const std::vector<FramePtr>& close_kfs,
                                       const MapPtr& map,
                                       const float sigma_threshold);

  void publishSeedsBinary(const MapPtr& map, const float sigma_threshold);

  void publishSeeds(const MapPtr& map);

  void publishSeedsAsPointcloud(const Frame& frame, bool only_converged_seeds,
                                bool reset_pc_before_publishing = true);

  void publishVelocity(const Eigen::Vector3d& velocity_imu,
                       const uint64_t timestamp);

  void publishMapRegion(const std::vector<FramePtr>& frames);

  void publishKeyframeWithPoints(const FramePtr& frame,
                                 const uint64_t timestamp,
                                 const double marker_scale = 0.05);

  void publishActiveKeyframes(const std::vector<FramePtr>& active_kfs);

  void exportToDense(const FrameBundlePtr& frame_bundle);

  void publishSeedsUncertainty(const MapPtr& map);

  void visualizeCoordinateFrames(const Transformation& T_world_cam);
  
  void writeCaptionStr(cv::Mat img);
};

}  // end namespace svo
