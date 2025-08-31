#include "svo/ceres_backend_publisher.hpp"

#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <sensor_msgs/Imu.h>
#include <visualization_msgs/Marker.h>

#include <vikit/output_helper.h>
#include <vikit/params_helper.h>

namespace
{
template <typename T>
void normalizeVector(const std::vector<T>& in, std::vector<float>* out)
{
  auto res = std::minmax_element(in.begin(), in.end());
  const T min = *res.first;
  const T max = *res.second;
  const float dist = static_cast<float>(max - min);

  out->resize(in.size());
  for (size_t i = 0; i < in.size(); i++)
  {
    (*out)[i] = (in[i] - min) / dist;
  }
}
}

namespace svo
{
CeresBackendPublisher::CeresBackendPublisher(
    const ros::NodeHandle& nh_private,
    const std::shared_ptr<ceres_backend::Map>& map_ptr)
  : pnh_(nh_private)
  , map_ptr_(map_ptr)
{
  pub_imu_pose_ = pnh_.advertise<geometry_msgs::PoseWithCovarianceStamped>(
      "backend_pose_imu", 10);
  pub_imu_vel_ = pnh_.advertise<geometry_msgs::Vector3Stamped>(
      "backend_imu_vel", 10);
  pub_imu_bias_ = pnh_.advertise<geometry_msgs::TwistStamped>(
      "backend_imu_bias", 10);
  pub_imu_pose_viz_ =
      pnh_.advertise<geometry_msgs::PoseStamped>("backend_pose_imu_viz", 10);
  pub_points_ =
      pnh_.advertise<PointCloud>("backend_points", 10);
  pub_dynamics_ = pnh_.advertise<geometry_msgs::Vector3Stamped>(
      "external_force", 10);
  pub_dynamics_meas_ = pnh_.advertise<sensor_msgs::Imu>(
      "dynamic_measurements", 10);
}

void CeresBackendPublisher::publish(const ViNodeState& state,
                                    const int64_t timestamp,
                                    const int32_t seq)
{
  publishImuPoseVelBias(state, timestamp, seq);
  publishBackendLandmarks(timestamp);
}

void CeresBackendPublisher::publishWithDynamics(
  const ViNodeState& state, const int64_t timestamp_state, 
  const Eigen::Matrix<double, 3, 1>& Fe, const int64_t timestamp_force, 
  const DynamicsMeasurements& m,
  const bool pub_body_rates,
  const int32_t seq)
{
  publishImuPoseVelBias(state, timestamp_state, seq);
  publishBackendLandmarks(timestamp_state);
  publishDynamics(m, pub_body_rates, Fe, timestamp_force, seq);
}

void CeresBackendPublisher::publishImuPose(const ViNodeState& state,
                                           const int64_t timestamp,
                                           const int32_t seq)
{
  // Trace state
  state_ = state;

  {
    std::lock_guard<std::mutex> lock(mutex_frame_id_);
    state_frame_id_ = BundleId(seq);
  }

  size_t n_pose_sub = pub_imu_pose_.getNumSubscribers();
  size_t n_pose_viz_sub = pub_imu_pose_viz_.getNumSubscribers();
  if (n_pose_sub == 0 && n_pose_viz_sub == 0)
  {
    return;
  }
  VLOG(100) << "Publish IMU Pose";

  Eigen::Quaterniond q = state.get_T_W_B().getRotation().toImplementation();
  Eigen::Vector3d p = state.get_T_W_B().getPosition();
  ros::Time time = ros::Time().fromNSec(timestamp);

  if (n_pose_sub > 0)
  {
    geometry_msgs::PoseWithCovarianceStampedPtr msg_pose(
        new geometry_msgs::PoseWithCovarianceStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.pose.position.x = p[0];
    msg_pose->pose.pose.position.y = p[1];
    msg_pose->pose.pose.position.z = p[2];
    msg_pose->pose.pose.orientation.x = q.x();
    msg_pose->pose.pose.orientation.y = q.y();
    msg_pose->pose.pose.orientation.z = q.z();
    msg_pose->pose.pose.orientation.w = q.w();
    for (size_t i = 0; i < 36; ++i)
      msg_pose->pose.covariance[i] = 0;
    pub_imu_pose_.publish(msg_pose);
  }

  if (n_pose_viz_sub > 0)
  {
    geometry_msgs::PoseStampedPtr msg_pose(new geometry_msgs::PoseStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.position.x = p[0];
    msg_pose->pose.position.y = p[1];
    msg_pose->pose.position.z = p[2];
    msg_pose->pose.orientation.x = q.x();
    msg_pose->pose.orientation.y = q.y();
    msg_pose->pose.orientation.z = q.z();
    msg_pose->pose.orientation.w = q.w();
    pub_imu_pose_viz_.publish(msg_pose);
  }
}

void CeresBackendPublisher::publishImuPoseVelBias(
  const ViNodeState& state, const int64_t timestamp, const int32_t seq)
{
  // Trace state
  state_ = state;

  {
    std::lock_guard<std::mutex> lock(mutex_frame_id_);
    state_frame_id_ = BundleId(seq);
  }

  size_t n_pose_sub = pub_imu_pose_.getNumSubscribers();
  size_t n_pose_viz_sub = pub_imu_pose_viz_.getNumSubscribers();
  if (n_pose_sub == 0 && n_pose_viz_sub == 0)
  {
    return;
  }
  VLOG(100) << "Publish IMU Pose";

  Eigen::Quaterniond q = state.get_T_W_B().getRotation().toImplementation();
  Eigen::Vector3d p = state.get_T_W_B().getPosition();
  Eigen::Vector3d v = state.get_W_v_B();
  Eigen::Vector3d ba = state.getAccBias();
  Eigen::Vector3d bg = state.getGyroBias();
  ros::Time time = ros::Time().fromNSec(timestamp);

  if (n_pose_sub > 0)
  {
    geometry_msgs::PoseWithCovarianceStampedPtr msg_pose(
        new geometry_msgs::PoseWithCovarianceStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.pose.position.x = p[0];
    msg_pose->pose.pose.position.y = p[1];
    msg_pose->pose.pose.position.z = p[2];
    msg_pose->pose.pose.orientation.x = q.x();
    msg_pose->pose.pose.orientation.y = q.y();
    msg_pose->pose.pose.orientation.z = q.z();
    msg_pose->pose.pose.orientation.w = q.w();
    for (size_t i = 0; i < 36; ++i)
    {
      msg_pose->pose.covariance[i] = 0;
    }

    // Add biases and velocity here for simple callback in ekf vio filter
    msg_pose->pose.covariance[0] = ba[0];
    msg_pose->pose.covariance[1] = ba[1];
    msg_pose->pose.covariance[2] = ba[2];
    msg_pose->pose.covariance[3] = bg[0];
    msg_pose->pose.covariance[4] = bg[1];
    msg_pose->pose.covariance[5] = bg[2];
    msg_pose->pose.covariance[6] = v[0];
    msg_pose->pose.covariance[7] = v[1];
    msg_pose->pose.covariance[8] = v[2];

    pub_imu_pose_.publish(msg_pose);
  }

  if (n_pose_viz_sub > 0)
  {
    geometry_msgs::PoseStampedPtr msg_pose(new geometry_msgs::PoseStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.position.x = p[0];
    msg_pose->pose.position.y = p[1];
    msg_pose->pose.position.z = p[2];
    msg_pose->pose.orientation.x = q.x();
    msg_pose->pose.orientation.y = q.y();
    msg_pose->pose.orientation.z = q.z();
    msg_pose->pose.orientation.w = q.w();
    pub_imu_pose_viz_.publish(msg_pose);
  }

  geometry_msgs::Vector3StampedPtr msg_vel(
        new geometry_msgs::Vector3Stamped);
  msg_vel->header.seq = seq;
  msg_vel->header.stamp = time;
  msg_vel->header.frame_id = kWorldFrame;
  msg_vel->vector.x = v[0];
  msg_vel->vector.y = v[1];
  msg_vel->vector.z = v[2];
  pub_imu_vel_.publish(msg_vel);

  geometry_msgs::TwistStampedPtr msg_bias(
        new geometry_msgs::TwistStamped);
  msg_bias->header.seq = seq;
  msg_bias->header.stamp = time;
  msg_bias->header.frame_id = kWorldFrame;
  msg_bias->twist.linear.x = ba[0];
  msg_bias->twist.linear.y = ba[1];
  msg_bias->twist.linear.z = ba[2];
  msg_bias->twist.angular.x = bg[0];
  msg_bias->twist.angular.y = bg[1];
  msg_bias->twist.angular.z = bg[2];
  pub_imu_bias_.publish(msg_bias);
}

void CeresBackendPublisher::publishImuPoseAndBias(
  const ViNodeState& state, const int64_t timestamp, const int32_t seq)
{
  // Trace state
  state_ = state;

  {
    std::lock_guard<std::mutex> lock(mutex_frame_id_);
    state_frame_id_ = BundleId(seq);
  }

  size_t n_pose_sub = pub_imu_pose_.getNumSubscribers();
  size_t n_pose_viz_sub = pub_imu_pose_viz_.getNumSubscribers();
  if (n_pose_sub == 0 && n_pose_viz_sub == 0)
  {
    return;
  }
  VLOG(100) << "Publish IMU Pose";

  Eigen::Quaterniond q = state.get_T_W_B().getRotation().toImplementation();
  Eigen::Vector3d p = state.get_T_W_B().getPosition();
  Eigen::Vector3d ba = state.getAccBias();
  Eigen::Vector3d bg = state.getGyroBias();
  ros::Time time = ros::Time().fromNSec(timestamp);

  if (n_pose_sub > 0)
  {
    geometry_msgs::PoseWithCovarianceStampedPtr msg_pose(
        new geometry_msgs::PoseWithCovarianceStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.pose.position.x = p[0];
    msg_pose->pose.pose.position.y = p[1];
    msg_pose->pose.pose.position.z = p[2];
    msg_pose->pose.pose.orientation.x = q.x();
    msg_pose->pose.pose.orientation.y = q.y();
    msg_pose->pose.pose.orientation.z = q.z();
    msg_pose->pose.pose.orientation.w = q.w();
    for (size_t i = 0; i < 36; ++i)
      msg_pose->pose.covariance[i] = 0;
    pub_imu_pose_.publish(msg_pose);
  }

  if (n_pose_viz_sub > 0)
  {
    geometry_msgs::PoseStampedPtr msg_pose(new geometry_msgs::PoseStamped);
    msg_pose->header.seq = seq;
    msg_pose->header.stamp = time;
    msg_pose->header.frame_id = kWorldFrame;
    msg_pose->pose.position.x = p[0];
    msg_pose->pose.position.y = p[1];
    msg_pose->pose.position.z = p[2];
    msg_pose->pose.orientation.x = q.x();
    msg_pose->pose.orientation.y = q.y();
    msg_pose->pose.orientation.z = q.z();
    msg_pose->pose.orientation.w = q.w();
    pub_imu_pose_viz_.publish(msg_pose);
  }

  geometry_msgs::TwistStampedPtr msg_bias(
        new geometry_msgs::TwistStamped);
  msg_bias->header.seq = seq;
  msg_bias->header.stamp = time;
  msg_bias->header.frame_id = kWorldFrame;
  msg_bias->twist.linear.x = ba[0];
  msg_bias->twist.linear.y = ba[1];
  msg_bias->twist.linear.z = ba[2];
  msg_bias->twist.angular.x = bg[0];
  msg_bias->twist.angular.y = bg[1];
  msg_bias->twist.angular.z = bg[2];
  pub_imu_bias_.publish(msg_bias);
}

void CeresBackendPublisher::publishBackendLandmarks(
    const int64_t timestamp) const
{
  if (pub_points_.getNumSubscribers() == 0)
  {
    return;
  }

  // get all landmarks
  const std::unordered_map<
      uint64_t, std::shared_ptr<ceres_backend::ParameterBlock> >& idmap =
      map_ptr_->idToParameterBlockMap();
  size_t n_pts = 0;
  std::vector<const double*> landmark_pointers;
  std::vector<uint64_t> point_ids;
  for (auto& it : idmap)
  {
    if (it.second->typeInfo() == "HomogeneousPointParameterBlock" &&
        !it.second->fixed())
    {
      n_pts++;
      landmark_pointers.push_back(it.second->parameters());
      point_ids.push_back(it.second->id());
    }
  }

  if (n_pts < 5)
  {
    return;
  }

  std::vector<float> intensities;
  normalizeVector(point_ids, &intensities);

  // point clound to publish
  PointCloud pc;
  ros::Time pub_time;
  pub_time.fromNSec(timestamp);
  pcl_conversions::toPCL(pub_time, pc.header.stamp);
  pc.header.frame_id = kWorldFrame;
  pc.reserve(n_pts);
  for(size_t i = 0; i < landmark_pointers.size(); i++)
  {
    const auto p = landmark_pointers[i];
    PointType pt;
    pt.intensity = intensities[i];
    pt.x = p[0];
    pt.y = p[1];
    pt.z = p[2];
    pc.push_back(pt);
  }
  pub_points_.publish(pc);
}

void CeresBackendPublisher::publishDynamics(
  const DynamicsMeasurements& m,
  const bool pub_body_rates,
  const Eigen::Matrix<double, 3, 1>& Fe, const int64_t Fe_timestamp, 
  const int32_t seq)
{
  // Dynamics measurements
  if (m.size() > 0)
  {
    for (auto it=m.rbegin(); it!=m.rend(); ++it)
    {
      if (it->timestamp_ > dynamic_measurement_latest_time_)
      {
        ros::Time time = ros::Time().fromSec(it->timestamp_);

        sensor_msgs::ImuPtr msg(new sensor_msgs::Imu);
        msg->header.stamp = time;
        msg->header.frame_id = kWorldFrame;
        msg->linear_acceleration.x = it->collective_thrust_[0];
        msg->linear_acceleration.y = it->collective_thrust_[1];
        msg->linear_acceleration.z = it->collective_thrust_[2];
        if (pub_body_rates)
        {
          msg->angular_velocity.x = it->body_rates_[0];
          msg->angular_velocity.y = it->body_rates_[1];
          msg->angular_velocity.z = it->body_rates_[2];
        }
        else
        {
          msg->angular_velocity.x = it->gyro_[0];
          msg->angular_velocity.y = it->gyro_[1];
          msg->angular_velocity.z = it->gyro_[2];
        }
        pub_dynamics_meas_.publish(msg);
      }
    }
    dynamic_measurement_latest_time_ = m.front().timestamp_;
  }
  
  // external force
  ros::Time time = ros::Time().fromNSec(Fe_timestamp);

  geometry_msgs::Vector3StampedPtr msg(new geometry_msgs::Vector3Stamped);
  msg->header.seq = seq;
  msg->header.stamp = time;
  msg->header.frame_id = kWorldFrame;
  msg->vector.x = Fe[0];
  msg->vector.y = Fe[1];
  msg->vector.z = Fe[2];
  pub_dynamics_.publish(msg);
}

}  // namespace svo
