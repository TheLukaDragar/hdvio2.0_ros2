/*********************************************************************************
 *  HDVIO2.0
 *  Code based on:
 *  OKVIS - Open Keyframe-based Visual-Inertial SLAM
 *  Copyright (c) 2015, Autonomous Systems Lab / ETH Zurich
 *  Copyright (c) 2016, ETH Zurich, Wyss Zurich, Zurich Eye
 *  Copyright (c) 2022, Robotics and Perception Group, University of Zurich
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of Autonomous Systems Lab / ETH Zurich nor the names of
 *     its contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  Created on: Nov 1, 2022
 *      Author: Giovanni Cioffi (cioffi@ifi.uzh.ch)
 *********************************************************************************/

/**
 * @file DynamicsError.cpp
 * @brief Source file for the DynamicsError class.
 * @author Giovanni Cioffi
 */

#include "svo/ceres_backend/dynamics_error.hpp"

#include <svo/vio_common/matrix.hpp>
#include <svo/vio_common/matrix_operations.hpp>
#include <svo/common/conversions.h>

#include "svo/ceres_backend/pose_local_parameterization.hpp"

namespace svo {
namespace ceres_backend {

// Construct with measurements and parameters.
DynamicsError::DynamicsError(
    const DynamicsMeasurements &dynamics_measurements, 
    const DynamicsHandlerOptions& dynamics_parameters,
    const double& t_0, const double& t_1)
{  
  std::lock_guard<std::mutex> lock(preintegration_mutex_);
  setDynamicsMeasurements(dynamics_measurements);
  setDynamicsParameters(dynamics_parameters);
  setT0(t_0);
  setT1(t_1);

  DEBUG_CHECK(t0_ >= dynamics_measurements.back().timestamp_)
      << "First dynamics measurement included in DynamicsError is not old enough!";
  DEBUG_CHECK(t1_ <= dynamics_measurements.front().timestamp_)
      << "Last dynamics measurement included in DynamicsError is not new enough!";
}

// Propagates pose, speeds and biases with given IMU measurements.
int DynamicsError::redoPreintegration(
    const Transformation& /*T_WS*/, const SpeedAndBias& speed_and_biases) const
{
  // ensure unique access
  std::lock_guard<std::mutex> lock(preintegration_mutex_);

  // now the propagation
  double time = t0_;

  // sanity check:
  assert(dynamics_measurements_.back().timestamp_<=time);
  if (!(dynamics_measurements_.front().timestamp_>=t1_))
  {
    return -1;  // nothing to do...
  }

  // increments (initialise with identity)
  Delta_q_ = Eigen::Quaterniond(1, 0, 0, 0);
  C_integral_ = Eigen::Matrix3d::Zero();
  C_doubleintegral_ = Eigen::Matrix3d::Zero();
  acc_integral_ = Eigen::Vector3d::Zero();
  acc_doubleintegral_ = Eigen::Vector3d::Zero();

  // cross matrix accumulation
  cross_ = Eigen::Matrix3d::Zero();

  // sub-Jacobians
  dalpha_db_g_ = Eigen::Matrix3d::Zero();
  dv_db_g_ = Eigen::Matrix3d::Zero();
  dp_db_g_ = Eigen::Matrix3d::Zero();

  // the Jacobian of the increment (w/o biases)
  P_delta_ = Eigen::Matrix<double, 12, 12>::Zero();

  double delta_t = 0;
  bool has_started = false;
  bool last_iteration = false;
  int n_integrated = 0;
  for (size_t i = dynamics_measurements_.size()-1; i != 0u; --i)
  {
    Eigen::Vector3d omega_S_0 = dynamics_measurements_[i].gyro_;
    Eigen::Vector3d acc_S_0 = dynamics_measurements_[i].collective_thrust_;
    Eigen::Vector3d omega_S_1 = dynamics_measurements_[i-1].gyro_;
    Eigen::Vector3d acc_S_1 = dynamics_measurements_[i-1].collective_thrust_;
    double nexttime = dynamics_measurements_[i-1].timestamp_;

    // time delta
    double dt = nexttime - time;

    if (t1_ < nexttime)
    {
      double interval = nexttime - dynamics_measurements_[i].timestamp_;
      nexttime = t1_;
      last_iteration = true;
      dt = nexttime - time;
      const double r = dt / interval;
      omega_S_1 = ((1.0 - r) * omega_S_0 + r * omega_S_1).eval();
      acc_S_1 = ((1.0 - r) * acc_S_0 + r * acc_S_1).eval();
    }

    if (dt <= 0.0)
    {
      continue;
    }
    delta_t += dt;

    if (!has_started)
    {
      has_started = true;
      const double r = dt / (nexttime - dynamics_measurements_[i].timestamp_);
      omega_S_0 = (r * omega_S_0 + (1.0 - r) * omega_S_1).eval();
      acc_S_0 = (r * acc_S_0 + (1.0 - r) * acc_S_1).eval();
    }

    double sigma_g_c = dynamics_parameters_.sigma_bodyrates_c;
    double sigma_thrx_c = dynamics_parameters_.sigma_thrustx_c;
    double sigma_thry_c = dynamics_parameters_.sigma_thrusty_c;
    double sigma_thrz_c = dynamics_parameters_.sigma_thrustz_c;

    // actual propagation
    // orientation:
    Eigen::Quaterniond dq;
    const Eigen::Vector3d omega_S_true =
        (0.5 * (omega_S_0 + omega_S_1) - speed_and_biases.segment<3>(3));
    const double theta_half = omega_S_true.norm() * 0.5 * dt;
    const double sinc_theta_half = dyn_sinc(theta_half);
    const double cos_theta_half = cos(theta_half);
    dq.vec() = sinc_theta_half * omega_S_true * 0.5 * dt;
    dq.w() = cos_theta_half;
    Eigen::Quaterniond Delta_q_1 = Delta_q_ * dq;
    // rotation matrix integral:
    const Eigen::Matrix3d C = Delta_q_.toRotationMatrix();
    const Eigen::Matrix3d C_1 = Delta_q_1.toRotationMatrix();
    const Eigen::Vector3d acc_S_true = 0.5 * (acc_S_0 + acc_S_1);
    const Eigen::Matrix3d C_integral_1 = C_integral_ + 0.5 * (C + C_1) * dt;
    const Eigen::Vector3d acc_integral_1 =
        acc_integral_ + 0.5 * (C + C_1) * acc_S_true * dt;
    // rotation matrix double integral:
    C_doubleintegral_ += C_integral_ * dt + 0.25 * (C + C_1) * dt * dt;
    acc_doubleintegral_ +=
        acc_integral_ * dt + 0.25 * (C + C_1) * acc_S_true * dt * dt;

    // Jacobian parts
    dalpha_db_g_ += C_1 * expmapDerivativeSO3(omega_S_true * dt) * dt;
    const Eigen::Matrix3d cross_1 =
        dq.inverse().toRotationMatrix() * cross_
        + expmapDerivativeSO3(omega_S_true * dt) * dt;
    const Eigen::Matrix3d acc_S_x = skewSymmetric(acc_S_true);
    Eigen::Matrix3d dv_db_g_1 =
        dv_db_g_ + 0.5 * dt * (C * acc_S_x * cross_ + C_1 * acc_S_x * cross_1);
    dp_db_g_ +=
        dt * dv_db_g_
        + 0.25 * dt * dt * (C * acc_S_x * cross_ + C_1 * acc_S_x * cross_1);

    // covariance propagation
    Eigen::Matrix<double, 12, 12> F_delta =
        Eigen::Matrix<double, 12, 12>::Identity();
    // transform
    F_delta.block<3, 3>(0, 3) =
        -skewSymmetric(acc_integral_ * dt
                       + 0.25 * (C + C_1) * acc_S_true * dt * dt);
    F_delta.block<3, 3>(0, 6) = Eigen::Matrix3d::Identity() * dt;
    F_delta.block<3, 3>(0, 9) =
        dt * dv_db_g_
        + 0.25 * dt * dt * (C * acc_S_x * cross_ + C_1 * acc_S_x * cross_1);
    F_delta.block<3, 3>(3, 9) = -dt * C_1;
    F_delta.block<3, 3>(6, 3) =
        -skewSymmetric(0.5 * (C + C_1) * acc_S_true * dt);
    F_delta.block<3, 3>(6, 9) =
        0.5 * dt * (C * acc_S_x * cross_ + C_1 * acc_S_x * cross_1);
    P_delta_ = F_delta * P_delta_ * F_delta.transpose();
    // add noise. Note that transformations with rotation matrices can be
    // ignored, since the noise is isotropic.
    const double sigma2_dalpha = dt * sigma_g_c * sigma_g_c;
    P_delta_(3, 3) += sigma2_dalpha;
    P_delta_(4, 4) += sigma2_dalpha;
    P_delta_(5, 5) += sigma2_dalpha;
    const double sigma2_vx = dt * sigma_thrx_c * sigma_thrx_c;
    const double sigma2_vy = dt * sigma_thry_c * sigma_thry_c;
    const double sigma2_vz = dt * sigma_thrz_c * sigma_thrz_c;
    P_delta_(6, 6) += sigma2_vx;
    P_delta_(7, 7) += sigma2_vy;
    P_delta_(8, 8) += sigma2_vz;
    const double sigma2_px = 0.5 * dt * dt * sigma2_vx;
    const double sigma2_py = 0.5 * dt * dt * sigma2_vy;
    const double sigma2_pz = 0.5 * dt * dt * sigma2_vz;
    P_delta_(0, 0) += sigma2_px;
    P_delta_(1, 1) += sigma2_py;
    P_delta_(2, 2) += sigma2_pz;
    const double sigma2_b_g =
        dt * dynamics_parameters_.sigma_gyro_bias_c * dynamics_parameters_.sigma_gyro_bias_c;
    P_delta_(9, 9) += sigma2_b_g;
    P_delta_(10, 10) += sigma2_b_g;
    P_delta_(11, 11) += sigma2_b_g;

    // memory shift
    Delta_q_ = Delta_q_1;
    C_integral_ = C_integral_1;
    acc_integral_ = acc_integral_1;
    cross_ = cross_1;
    dv_db_g_ = dv_db_g_1;
    time = nexttime;

    ++n_integrated;

    if (last_iteration)
      break;

  }

  // store the reference (linearisation) point
  gyro_bias_ref_ = speed_and_biases.segment<3>(3);

  // get the weighting:
  // enforce symmetric
  P_delta_ = 0.5 * P_delta_ + 0.5 * P_delta_.transpose().eval();

  // calculate inverse
  information_ = P_delta_.inverse();
  information_ = 0.5 * information_ + 0.5 * information_.transpose().eval();

  // square root
  Eigen::LLT<information_t> lltOfInformation(information_);
  square_root_information_ = lltOfInformation.matrixL().transpose();

  return n_integrated;
}

// Propagates pose, speeds and biases with given IMU measurements.
int DynamicsError::propagation(const DynamicsMeasurements& dynamics_measurements,
                          const DynamicsHandlerOptions& dynamics_params,
                          Transformation& T_WS,
                          SpeedAndBias & speed_and_biases,
                          const double& t_start,
                          const double& t_end,
                          covariance_t* covariance,
                          jacobian_t* jacobian)
{
  // sanity check:
  assert(dynamics_measurements.back().timestamp_<= t_start);
  if (!(dynamics_measurements.front().timestamp_>= t_end))
  {
    assert(false);
    return -1;  // nothing to do...
  }

  // initial condition
  Eigen::Vector3d r_0 = T_WS.getPosition();
  Eigen::Quaterniond q_WS_0 = T_WS.getEigenQuaternion();
  Eigen::Matrix3d C_WS_0 = T_WS.getRotationMatrix();

  // increments (initialise with identity)
  Eigen::Quaterniond Delta_q(1,0,0,0);
  Eigen::Matrix3d C_integral = Eigen::Matrix3d::Zero();
  Eigen::Matrix3d C_doubleintegral = Eigen::Matrix3d::Zero();
  Eigen::Vector3d acc_integral = Eigen::Vector3d::Zero();
  Eigen::Vector3d acc_doubleintegral = Eigen::Vector3d::Zero();

  // cross matrix accumulatrion
  Eigen::Matrix3d cross = Eigen::Matrix3d::Zero();

  // sub-Jacobians
  Eigen::Matrix3d dalpha_db_g = Eigen::Matrix3d::Zero();
  Eigen::Matrix3d dv_db_g = Eigen::Matrix3d::Zero();
  Eigen::Matrix3d dp_db_g = Eigen::Matrix3d::Zero();

  // the Jacobian of the increment (w/o biases)
  Eigen::Matrix<double,12,12> P_delta = Eigen::Matrix<double,12,12>::Zero();

  double Delta_t = 0;
  bool has_started = false;
  int num_propagated = 0;

  double time = t_start;
  for (size_t i = dynamics_measurements.size()-1; i!=0u; --i)
  {
    Eigen::Vector3d omega_S_0 = dynamics_measurements[i].gyro_;
    Eigen::Vector3d acc_S_0 = dynamics_measurements[i].collective_thrust_;
    Eigen::Vector3d omega_S_1 = dynamics_measurements[i-1].gyro_;
    Eigen::Vector3d acc_S_1 = dynamics_measurements[i-1].collective_thrust_;
    double nexttime = dynamics_measurements[i-1].timestamp_;

    // time delta
    double dt = nexttime - time;

    if (t_end < nexttime)
    {
      double interval = nexttime - dynamics_measurements[i].timestamp_;
      nexttime = t_end;
      dt = nexttime - time;
      const double r = dt / interval;
      omega_S_1 = ((1.0 - r) * omega_S_0 + r * omega_S_1).eval();
      acc_S_1 = ((1.0 - r) * acc_S_0 + r * acc_S_1).eval();
    }

    if (dt <= 0.0)
    {
      continue;
    }
    Delta_t += dt;

    if (!has_started)
    {
      has_started = true;
      const double r = dt / (nexttime - dynamics_measurements[i].timestamp_);
      omega_S_0 = (r * omega_S_0 + (1.0 - r) * omega_S_1).eval();
      acc_S_0 = (r * acc_S_0 + (1.0 - r) * acc_S_1).eval();
    }

    double sigma_g_c = dynamics_params.sigma_bodyrates_c;
    double sigma_thrx_c = dynamics_params.sigma_thrustx_c;
    double sigma_thry_c = dynamics_params.sigma_thrusty_c;
    double sigma_thrz_c = dynamics_params.sigma_thrustz_c;

    // actual propagation
    // orientation:
    Eigen::Quaterniond dq;
    const Eigen::Vector3d omega_S_true =
        (0.5 *(omega_S_0+omega_S_1) - speed_and_biases.segment<3>(3));
    const double theta_half = omega_S_true.norm() * 0.5 * dt;
    const double sinc_theta_half = dyn_sinc(theta_half);
    const double cos_theta_half = cos(theta_half);
    dq.vec() = sinc_theta_half * omega_S_true * 0.5 * dt;
    dq.w() = cos_theta_half;
    Eigen::Quaterniond Delta_q_1 = Delta_q * dq;
    // rotation matrix integral:
    const Eigen::Matrix3d C = Delta_q.toRotationMatrix();
    const Eigen::Matrix3d C_1 = Delta_q_1.toRotationMatrix();
    const Eigen::Vector3d acc_S_true = 0.5 * (acc_S_0 + acc_S_1);
    const Eigen::Matrix3d C_integral_1 = C_integral + 0.5 *(C + C_1) * dt;
    const Eigen::Vector3d acc_integral_1 =
        acc_integral + 0.5 * (C + C_1) * acc_S_true * dt;
    // rotation matrix double integral:
    C_doubleintegral += C_integral * dt + 0.25 * (C + C_1) * dt * dt;
    acc_doubleintegral +=
        acc_integral * dt + 0.25 * (C + C_1) * acc_S_true * dt * dt;

    // Jacobian parts
    dalpha_db_g += dt * C_1;
    const Eigen::Matrix3d cross_1 =
        dq.inverse().toRotationMatrix()* cross +
        expmapDerivativeSO3(omega_S_true * dt)* dt;
    const Eigen::Matrix3d acc_S_x = skewSymmetric(acc_S_true);
    Eigen::Matrix3d dv_db_g_1 =
        dv_db_g + 0.5 * dt * (C * acc_S_x * cross + C_1 * acc_S_x * cross_1);
    dp_db_g +=
        dt * dv_db_g
        + 0.25 * dt * dt *(C * acc_S_x * cross + C_1 * acc_S_x * cross_1);

    // covariance propagation
    // note: update of pos and vel wrt to acc bias has been removed
    if (covariance)
    {
      Eigen::Matrix<double,12,12> F_delta =
          Eigen::Matrix<double,12,12>::Identity();
      // transform
      F_delta.block<3,3>(0,3) =
          -skewSymmetric(acc_integral * dt + 0.25 * (C + C_1)
                         * acc_S_true * dt * dt);
      F_delta.block<3,3>(0,6) = Eigen::Matrix3d::Identity()* dt;
      F_delta.block<3,3>(0,9) =
          dt *dv_db_g
          + 0.25 * dt * dt *(C * acc_S_x * cross + C_1 * acc_S_x * cross_1);
      F_delta.block<3,3>(3,9) = -dt * C_1;
      F_delta.block<3,3>(6,3) =
          -skewSymmetric(0.5 *(C + C_1) * acc_S_true * dt);
      F_delta.block<3,3>(6,9) =
          0.5 * dt *(C * acc_S_x * cross + C_1 * acc_S_x * cross_1);
      P_delta = F_delta * P_delta * F_delta.transpose();
      // add noise. Note that transformations with rotation matrices can be
      // ignored, since the noise is isotropic.
      //F_tot = F_delta*F_tot;
      const double sigma2_dalpha = dt * sigma_g_c * sigma_g_c;
      P_delta(3,3) += sigma2_dalpha;
      P_delta(4,4) += sigma2_dalpha;
      P_delta(5,5) += sigma2_dalpha;
      const double sigma2_vx = dt * sigma_thrx_c * sigma_thrx_c;
      const double sigma2_vy = dt * sigma_thry_c * sigma_thry_c;
      const double sigma2_vz = dt * sigma_thrz_c * sigma_thrz_c;
      P_delta(6, 6) += sigma2_vx;
      P_delta(7, 7) += sigma2_vy;
      P_delta(8, 8) += sigma2_vz;
      const double sigma2_px = 0.5 * dt * dt * sigma2_vx;
      const double sigma2_py = 0.5 * dt * dt * sigma2_vy;
      const double sigma2_pz = 0.5 * dt * dt * sigma2_vz;
      P_delta(0, 0) += sigma2_px;
      P_delta(1, 1) += sigma2_py;
      P_delta(2, 2) += sigma2_pz;
      const double sigma2_b_g =
          dt * dynamics_params.sigma_gyro_bias_c * dynamics_params.sigma_gyro_bias_c;
      P_delta(9,9)   += sigma2_b_g;
      P_delta(10,10) += sigma2_b_g;
      P_delta(11,11) += sigma2_b_g;
    }

    // memory shift
    Delta_q = Delta_q_1;
    C_integral = C_integral_1;
    acc_integral = acc_integral_1;
    cross = cross_1;
    dv_db_g = dv_db_g_1;
    time = nexttime;

    ++num_propagated;

    if (nexttime == t_end)
      break;

  }

  // actual propagation output:
  const Eigen::Vector3d g_W = 9.8082 * Eigen::Vector3d(0, 0, 1.0);
  T_WS =
      Transformation(
        r_0 + speed_and_biases.head<3>() * Delta_t
        + C_WS_0 * (acc_doubleintegral/*-C_doubleintegral*speedAndBiases.segment<3>(6)*/)
        - 0.5 * g_W * Delta_t * Delta_t,
        q_WS_0 * Delta_q);
  speed_and_biases.head<3>() +=
      C_WS_0 * (acc_integral/*-C_integral*speedAndBiases.segment<3>(6)*/)
      - g_W * Delta_t;

  // assign Jacobian, if requested
  if (jacobian)
  {
    Eigen::Matrix<double,12,12>& F = *jacobian;
    F.setIdentity(); // holds for all states, including d/dalpha, d/db_g
    F.block<3,3>(0,3) = -skewSymmetric(C_WS_0 * acc_doubleintegral);
    F.block<3,3>(0,6) = Eigen::Matrix3d::Identity() * Delta_t;
    F.block<3,3>(0,9) = C_WS_0 * dp_db_g;
    F.block<3,3>(3,9) = -C_WS_0 * dalpha_db_g;
    F.block<3,3>(6,3) = -skewSymmetric(C_WS_0 * acc_integral);
    F.block<3,3>(6,9) = C_WS_0 * dv_db_g;
  }

  // overall covariance, if requested
  if (covariance)
  {
    Eigen::Matrix<double,12,12>& P = *covariance;
    // transform from local increments to actual states
    Eigen::Matrix<double,12,12> T = Eigen::Matrix<double,12,12>::Identity();
    T.topLeftCorner<3,3>() = C_WS_0;
    T.block<3,3>(3,3) = C_WS_0;
    T.block<3,3>(6,6) = C_WS_0;
    P = T * P_delta * T.transpose();
  }
  return num_propagated;
}

// This evaluates the error term and additionally computes the Jacobians.
bool DynamicsError::Evaluate(double const* const * parameters, double* residuals,
                        double** jacobians) const
{
  return EvaluateWithMinimalJacobians(parameters, residuals, jacobians, nullptr);
}

// This evaluates the error term and additionally computes
// the Jacobians in the minimal internal representation.
bool DynamicsError::EvaluateWithMinimalJacobians(double const* const * parameters,
                                            double* residuals,
                                            double** jacobians,
                                            double** jacobians_minimal) const
{
  // get poses
  const Transformation T_WS_0(
        Eigen::Vector3d(parameters[0][0], parameters[0][1], parameters[0][2]),
      Eigen::Quaterniond(parameters[0][6], parameters[0][3],
      parameters[0][4], parameters[0][5]));

  const Transformation T_WS_1(
        Eigen::Vector3d(parameters[2][0], parameters[2][1], parameters[2][2]),
      Eigen::Quaterniond(parameters[2][6], parameters[2][3],
      parameters[2][4], parameters[2][5]));

  // get speed and bias
  SpeedAndBias speed_and_biases_0;
  SpeedAndBias speed_and_biases_1;
  for (size_t i = 0; i < 9; ++i)
  {
    speed_and_biases_0[i] = parameters[1][i];
    speed_and_biases_1[i] = parameters[3][i];
  }

  // get external force (in body frame)
  ExternalForce fe_0;
  fe_0(0) = parameters[4][0];
  fe_0(1) = parameters[4][1]; 
  fe_0(2) = parameters[4][2];

  // this will NOT be changed:
  const Eigen::Matrix3d C_WS_0 = T_WS_0.getRotationMatrix();
  const Eigen::Matrix3d C_S0_W = C_WS_0.transpose();

  // call the propagation
  const double delta_t = t1_ - t0_;
  Eigen::Matrix<double, 3, 1> Delta_bg;
  // ensure unique access
  {
    std::lock_guard<std::mutex> lock(preintegration_mutex_);
    Delta_bg = speed_and_biases_0.segment<3>(3) - gyro_bias_ref_;
  }
  // only check change in gyroscope bias
  redo_ = redo_ || (Delta_bg.norm() * delta_t > 0.0001);
  if (redo_)
  {
    redoPreintegration(T_WS_0, speed_and_biases_0);
    redoCounter_++;
    Delta_bg.setZero();
    redo_ = false;
    /*if (redoCounter_ > 1) {
      std::cout << "pre-integration no. " << redoCounter_ << std::endl;
    }*/
  }

  // actual propagation output:
  {
    std::lock_guard<std::mutex> lock(preintegration_mutex_);
    // this is a bit stupid, but shared read-locks only come in C++14
    const Eigen::Vector3d g_W = Eigen::Vector3d(0, 0, 9.8082);

    // assign Jacobian w.r.t. x0
    Eigen::Matrix<double,12,12> F0 =
        Eigen::Matrix<double,12,12>::Identity(); // holds for d/db_g
    const Eigen::Vector3d delta_p_est_W =
        T_WS_0.getPosition() - T_WS_1.getPosition()
        + speed_and_biases_0.head<3>()* delta_t - 0.5 * g_W* delta_t * delta_t;
    const Eigen::Vector3d delta_v_est_W = speed_and_biases_0.head<3>()
        - speed_and_biases_1.head<3>() - g_W * delta_t;
    const Eigen::Quaterniond Dq =
        dyn_deltaQ(-dalpha_db_g_*Delta_bg.head<3>())*Delta_q_;
    F0.block<3,3>(0,0) = C_S0_W;
    F0.block<3,3>(0,3) = C_S0_W * skewSymmetric(delta_p_est_W);
    F0.block<3,3>(0,6) = C_S0_W * Eigen::Matrix3d::Identity()* delta_t;
    F0.block<3,3>(0,9) = dp_db_g_;
    F0.block<3,3>(3,3) =
        (quaternionPlusMatrix(Dq * T_WS_1.getEigenQuaternion().inverse()) *
         quaternionOplusMatrix(T_WS_0.getEigenQuaternion())).topLeftCorner<3,3>();
    F0.block<3,3>(3,9) =
        (quaternionOplusMatrix(T_WS_1.getEigenQuaternion().inverse() *
                               T_WS_0.getEigenQuaternion()) *
         quaternionOplusMatrix(Dq)).topLeftCorner<3,3>() * (-dalpha_db_g_);
    F0.block<3,3>(6,3) = C_S0_W * skewSymmetric(delta_v_est_W);
    F0.block<3,3>(6,6) = C_S0_W;
    F0.block<3,3>(6,9) = dv_db_g_;

    // assign Jacobian w.r.t. x1
    Eigen::Matrix<double,12,12> F1 =
        -Eigen::Matrix<double,12,12>::Identity(); // holds for the biases
    F1.block<3,3>(0,0) = -C_S0_W;
    F1.block<3,3>(3,3) =
        -(quaternionPlusMatrix(Dq) *
          quaternionOplusMatrix(T_WS_0.getEigenQuaternion()) *
          quaternionPlusMatrix(T_WS_1.getEigenQuaternion().inverse()))
        .topLeftCorner<3,3>();
    F1.block<3,3>(6,6) = -C_S0_W;

    // the full state error vector
    Eigen::Matrix<double, 12, 1> fullstate_error;
    fullstate_error.segment<3>(0) =
        C_S0_W * delta_p_est_W + 0.5 * fe_0 * delta_t * delta_t + 
        acc_doubleintegral_ + F0.block<3,3>(0,9)*Delta_bg;
    fullstate_error.segment<3>(3) =
        2.0 * (Dq * (T_WS_1.getEigenQuaternion().inverse() *
                     T_WS_0.getEigenQuaternion())).vec();
    fullstate_error.segment<3>(6) =
        C_S0_W * delta_v_est_W + fe_0 * delta_t + 
        acc_integral_ + F0.block<3,3>(6,9)*Delta_bg;
    fullstate_error.tail<3>() = speed_and_biases_0.segment<3>(3) - speed_and_biases_1.segment<3>(3);

    // error weighting
    Eigen::Matrix<double, 12, 1> fullstate_weighted_error(Eigen::Matrix<double, 12, 1>::Zero());
    fullstate_weighted_error = square_root_information_ * fullstate_error;

    Eigen::Map<Eigen::Matrix<double, 9, 1> > weighted_error(residuals);
    weighted_error.segment<3>(0) = fullstate_weighted_error.segment<3>(0);
    weighted_error.segment<3>(3) = fullstate_weighted_error.segment<3>(6);
    const double sqrt_info_ext_force_x = 1.0 / dynamics_parameters_.sigma_external_force.x();
    const double sqrt_info_ext_force_y = 1.0 / dynamics_parameters_.sigma_external_force.y();
    const double sqrt_info_ext_force_z = 1.0 / dynamics_parameters_.sigma_external_force.z();
    Eigen::Matrix3d square_root_information_ext_force(Eigen::Matrix3d::Identity());
    square_root_information_ext_force(0, 0) = sqrt_info_ext_force_x;
    square_root_information_ext_force(1, 1) = sqrt_info_ext_force_y;
    square_root_information_ext_force(2, 2) = sqrt_info_ext_force_z;
    weighted_error.segment<3>(6) = square_root_information_ext_force * fe_0;

    // get the Jacobians
    if (jacobians != nullptr)
    {
      if (jacobians[0] != nullptr)
      {
        // Jacobian w.r.t. minimal perturbance
        Eigen::Matrix<double, 12, 6> J0full_minimal =
            square_root_information_ * F0.block<12, 6>(0, 0);
        Eigen::Matrix<double, 9, 6> J0_minimal(Eigen::Matrix<double, 9, 6>::Zero());
        J0_minimal.block<3,6>(0,0) = J0full_minimal.block<3,6>(0,0);
        J0_minimal.block<3,6>(3,0) = J0full_minimal.block<3,6>(6,0);

        // pseudo inverse of the local parametrization Jacobian:
        Eigen::Matrix<double, 6, 7, Eigen::RowMajor> J_lift;
        PoseLocalParameterization::liftJacobian(parameters[0], J_lift.data());

        // hallucinate Jacobian w.r.t. state
        Eigen::Map<Eigen::Matrix<double, 9, 7, Eigen::RowMajor> > J0(
              jacobians[0]);
        J0 = J0_minimal * J_lift;

        // if requested, provide minimal Jacobians
        if (jacobians_minimal != nullptr)
        {
          if (jacobians_minimal[0] != nullptr)
          {
            Eigen::Map<Eigen::Matrix<double, 9, 6, Eigen::RowMajor> >
                J0_minimal_mapped(jacobians_minimal[0]);
            J0_minimal_mapped = J0_minimal;
          }
        }
      }
      if (jacobians[1] != nullptr)
      {
        Eigen::Matrix<double, 12, 6> J1full =
            square_root_information_ * F0.block<12, 6>(0, 6);
        // we need to include the acc bias since this is in the parameter block given to the class
        Eigen::Map<Eigen::Matrix<double, 9, 9, Eigen::RowMajor> >
            J1(jacobians[1]);
        J1 = Eigen::Matrix<double, 9, 9>::Zero();
        J1.block<3,6>(0,0) = J1full.block<3,6>(0,0);
        J1.block<3,6>(3,0) = J1full.block<3,6>(6,0);

        // if requested, provide minimal Jacobians
        if (jacobians_minimal != nullptr)
        {
          if (jacobians_minimal[1] != nullptr)
          {
            Eigen::Map<Eigen::Matrix<double, 9, 9, Eigen::RowMajor> >
                J1_minimal_mapped(jacobians_minimal[1]);
            J1_minimal_mapped = J1;
          }
        }
      }
      if (jacobians[2] != nullptr)
      {
        // Jacobian w.r.t. minimal perturbance
        Eigen::Matrix<double, 12, 6> J2full_minimal =
            square_root_information_ * F1.block<12, 6>(0, 0);
        Eigen::Matrix<double, 9, 6> J2_minimal(Eigen::Matrix<double, 9, 6>::Zero());
        J2_minimal.block<3,6>(0,0) = J2full_minimal.block<3,6>(0,0);
        J2_minimal.block<3,6>(3,0) = J2full_minimal.block<3,6>(6,0);

        // pseudo inverse of the local parametrization Jacobian:
        Eigen::Matrix<double, 6, 7, Eigen::RowMajor> J_lift;
        PoseLocalParameterization::liftJacobian(parameters[2], J_lift.data());

        // hallucinate Jacobian w.r.t. state
        Eigen::Map<Eigen::Matrix<double, 9, 7, Eigen::RowMajor> > J2(
              jacobians[2]);
        J2 = J2_minimal * J_lift;

        // if requested, provide minimal Jacobians
        if (jacobians_minimal != nullptr)
        {
          if (jacobians_minimal[2] != nullptr)
          {
            Eigen::Map<Eigen::Matrix<double, 9, 6, Eigen::RowMajor> >
                J2_minimal_mapped(jacobians_minimal[2]);
            J2_minimal_mapped = J2_minimal;
          }
        }
      }
      if (jacobians[3] != nullptr)
      {
        Eigen::Matrix<double, 12, 6> J3full =
            square_root_information_ * F1.block<12, 6>(0, 6);
        // we need to include the acc bias since this is in the parameter block given to the class
        Eigen::Map<Eigen::Matrix<double, 9, 9, Eigen::RowMajor> >
            J3(jacobians[3]);
        J3 = Eigen::Matrix<double, 9, 9>::Zero();
        J3.block<3,6>(0,0) = J3full.block<3,6>(0,0);
        J3.block<3,6>(3,0) = J3full.block<3,6>(6,0);

        // if requested, provide minimal Jacobians
        if (jacobians_minimal != nullptr)
        {
          if (jacobians_minimal[3] != nullptr)
          {
            Eigen::Map<Eigen::Matrix<double, 9, 9, Eigen::RowMajor> >
                J3_minimal_mapped(jacobians_minimal[3]);
            J3_minimal_mapped = J3;
          }
        }
      }
      if (jacobians[4] != nullptr)
      { 
        // this is the jacobian wrt the external force parameter
        Eigen::Map<Eigen::Matrix<double, 9, 3, Eigen::RowMajor> >
            J4(jacobians[4]);
        J4= Eigen::Matrix<double, 9, 3>::Zero();
        J4.block<3,3>(0,0) = 
        0.5 * square_root_information_ext_force * delta_t * delta_t * Eigen::Matrix<double, 3, 3>::Identity();
        J4.block<3,3>(3,0) = 
        square_root_information_ext_force * delta_t * Eigen::Matrix<double, 3, 3>::Identity();
        J4.block<3,3>(6,0) = 
        square_root_information_ext_force * Eigen::Matrix<double, 3, 3>::Identity();

        // if requested, provide minimal Jacobians
        if (jacobians_minimal != nullptr)
        {
          if (jacobians_minimal[4] != nullptr)
          {
            Eigen::Map<Eigen::Matrix<double, 9, 3, Eigen::RowMajor> >
                J4_minimal_mapped(jacobians_minimal[4]);
            J4_minimal_mapped = J4;
          }
        }
      }
    }
  }
  return true;
}

}  // namespace ceres_backend
}  // namespace svo
