// This file is part of HDVIO2
//
// Copyright (C) 2022 Giovanni Cioffi <cioffi at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// This file is subject to the terms and conditions defined in the file
// 'LICENSE', which is part of this source code package.

#pragma once

#include <boost/circular_buffer.hpp>
#include <cmath>
#include <memory>          // std::shared_ptr
#include <mutex>           // std::mutex
#include <iostream>
#include <fstream>
#include <deque>
#include <map>
#include <svo/svo.h>
#include <svo/common/types.h>
#include <svo/opt_quad_angvel_spline.h>


namespace svo {

struct DynamicsMeasurement
{
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  double timestamp_; ///< In seconds.
  Eigen::Vector3d body_rates_;
  Eigen::Vector3d collective_thrust_;  // it's mass normalized
  Eigen::Vector3d collective_torque_;
  Eigen::Vector3d accel_;
  Eigen::Vector3d gyro_;
  DynamicsMeasurement() 
  {
    timestamp_ = -1.0; 
    collective_thrust_ = Eigen::Vector3d::Zero();
    body_rates_ = Eigen::Vector3d::Zero();
    accel_ = Eigen::Vector3d::Zero();
    gyro_ = Eigen::Vector3d::Zero();
    collective_torque_ = Eigen::Vector3d::Zero();
  }
  DynamicsMeasurement(
      const double timestamp,
      const Eigen::Vector3d& collective_thrust)
  {
    timestamp_ = timestamp; 
    collective_thrust_ = collective_thrust;

    body_rates_ = Eigen::Vector3d::Zero();
    accel_ = Eigen::Vector3d::Zero();
    gyro_ = Eigen::Vector3d::Zero();
    collective_torque_ = Eigen::Vector3d::Zero();
  }
  DynamicsMeasurement(
      const double timestamp,
      const Eigen::Vector3d& body_rates,
      const Eigen::Vector3d& collective_thrust)
  {
    timestamp_ = timestamp; 
    body_rates_ = body_rates;
    collective_thrust_ = collective_thrust;

    accel_ = Eigen::Vector3d::Zero();
    gyro_ = Eigen::Vector3d::Zero();
    collective_torque_ = Eigen::Vector3d::Zero();
  }
  DynamicsMeasurement(
      const double timestamp,
      const Eigen::Vector3d& body_rates,
      const Eigen::Vector3d& collective_thrust,
      const Eigen::Vector3d& accel)
  {
    timestamp_ = timestamp; 
    body_rates_ = body_rates;
    collective_thrust_ = collective_thrust;
    accel_ = accel;

    gyro_ = Eigen::Vector3d::Zero();
    collective_torque_ = Eigen::Vector3d::Zero();
  }
  DynamicsMeasurement(
      const double timestamp,
      const Eigen::Vector3d& body_rates,
      const Eigen::Vector3d& collective_thrust,
      const Eigen::Vector3d& accel,
      const Eigen::Vector3d& collective_torque)
  {
    timestamp_ = timestamp; 
    body_rates_ = body_rates;
    collective_thrust_ = collective_thrust;
    accel_ = accel;
    collective_torque_ = collective_torque;
  }

  DynamicsMeasurement(
      const double timestamp,
      const Eigen::Vector3d& body_rates,
      const Eigen::Vector3d& collective_thrust,
      const Eigen::Vector3d& accel,
      const Eigen::Vector3d& collective_torque,
      const Eigen::Vector3d& gyro)
  {
    timestamp_ = timestamp; 
    body_rates_ = body_rates;
    collective_thrust_ = collective_thrust;
    accel_ = accel;
    collective_torque_ = collective_torque;
    gyro_ = gyro;
  }
};

typedef std::deque<DynamicsMeasurement, 
Eigen::aligned_allocator<DynamicsMeasurement>> DynamicsMeasurements;

typedef boost::circular_buffer<DynamicsMeasurement> DynamicsNetInputs;

struct DynamicsHandlerOptions
{
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  bool use_dynamics = false;
  // [Not used for now] It has to be the same as the one used for training the network 
  double input_freq = 200.0; // [Hz] 
  // it has to be the same as the one used for training the network
  double thrust_net_sampling_freq = 200.0; // [Hz]
  // it has to be the same as the one used for training the network
  double thrust_net_input_window_length = 1.0; // [sec]
  // it has to be the same as the one used for training the network
  double torque_net_sampling_freq = 200.0; // [Hz]
  // it has to be the same as the one used for training the network
  double torque_net_input_window_length = 1.0; // [sec]
  bool use_learned_residuals = false;
  bool use_rotational_dynamics = false;
  std::string thrust_net_model_file = "thrust_net.onnx";
  std::string torque_net_model_file = "torque_net.onnx";
  // drone model config.
  double mass = 1.0; // [kg]
  Eigen::Vector3d r_fr = Eigen::Vector3d::Zero();
  Eigen::Vector3d r_br = Eigen::Vector3d::Zero();
  Eigen::Vector3d r_bl = Eigen::Vector3d::Zero();
  Eigen::Vector3d r_fl = Eigen::Vector3d::Zero();
  Eigen::Matrix3d Inertia = Eigen::Matrix3d::Identity();
  Eigen::Vector4d thrust_coeffs = Eigen::Vector4d::Zero();
  Eigen::Vector4d torque_coeffs = Eigen::Vector4d::Zero();
  
  int n_init_frames = 20;
  double sigma_bodyrates_c = 1e-3;
  // gyroscope bias noise (used if gyroscope measurements replace the bodyrate)
  double sigma_gyro_bias_c = 1e-4;
  // used if external_force_zero_mean = false
  double sigma_accel_c = 1e-3;
  double sigma_accel_bias_c = 1e-4;
  double sigma_thrustx_c = 1;
  double sigma_thrusty_c = 1;
  double sigma_thrustz_c = 1;
  // is False, use the model proposed in VID-Fusion
  bool external_force_zero_mean = true;
  Eigen::Vector3d sigma_external_force;
  bool publish_dynamics = false;

  // rotational dynamics
  double dt_knot_s = 0.01;
  double spline_length_s = 0.5;
  int max_iter = 10;
  double stop_thresh = 1e-6;
  // Residuals. 0: angular velocities, 1: angular accelerations (not supported), 2: angular velocities and torques, 3 torques
  int mode = 3;
  bool debug_info = false;

};

class DynamicsHandler
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  typedef std::shared_ptr<DynamicsHandler> Ptr;
  typedef std::mutex mutex_t;
  typedef std::unique_lock<mutex_t> ulock_t;

  DynamicsHandlerOptions options_;

  DynamicsHandler(const DynamicsHandlerOptions& options);
  ~DynamicsHandler();

  bool addDynamicsMeasurement(const DynamicsMeasurement& m);
  bool addNetInputs(const DynamicsMeasurements& m);
  // return true if buffer is full
  bool getNetInputs(std::deque<DynamicsMeasurement>& m, const std::string& type);
  int getNetInBufferSize();
  int getThrustNetInBufferSize();
  int getTorqueNetInBufferSize();

  /// Get measurements up to for the exact borders.
  /// Note that you have to provide the camera timestamp.
  /// Internally, given the calibration it corrects the timestamps for delays.
  bool getMeasurementsContainingEdges(const double frame_timestamp,  
  DynamicsMeasurements& extracted_measurements, const bool remove_measurements);

  /// Get measurements before the exact borders.
  bool getMeasurementsExcludingEdges(const double t0, const double t1,
  DynamicsMeasurements& extracted_measurements, const bool remove_measurements);

  /// Interpolate gyroscope measurements at thrust times
  bool sampleGyroscopeMeasurements(const ImuMeasurements& imu_measurements, 
  DynamicsMeasurements& dynamics_measurements);

  /// Interpolate gyroscope measurements at thrust times
  bool sampleAccelerometerMeasurements(const ImuMeasurements& imu_measurements, 
  DynamicsMeasurements& dynamics_measurements);

  /// Interpolate accel and gyro measurements at thrust times
  bool sampleImuMeasurements(const ImuMeasurements& imu_measurements, 
  DynamicsMeasurements& dynamics_measurements);

  /// Duplicate measurements at initial and final imu time
  /// This is necessary for integration in the ceres error function
  bool duplicateBorders(
    const ImuMeasurements& imu_measurements, 
    DynamicsMeasurements& dynamics_measurements);

  // Load measurements from file
  // dataset id : Blackbird [1], 
  bool loadDynamicsMeasurementsFromFile(
    const std::string& filename,
    const int dataset_id,
    const Eigen::Matrix3d& R_B_I);

private:
  mutable mutex_t measurements_mut_;
  DynamicsMeasurements measurements_; ///< Newest measurement is at the front of the list
  DynamicsNetInputs net_inputs_; ///< Oldest measurement is at the front of the list
  std::ofstream ofs_; //!< File stream for tracing the received measurments

  double thrust_dt_net_in_; // delta time between net inputs
  int thrust_buffer_size_net_in_; // net input buffer size
  double torque_dt_net_in_; // delta time between net inputs
  int torque_buffer_size_net_in_; // net input buffer size
  double dt_net_in_; // delta time between net inputs (the smallest is taken)
  int buffer_size_net_in_; // net input buffer size (the biggest is taken)
};

} // namespace svo
