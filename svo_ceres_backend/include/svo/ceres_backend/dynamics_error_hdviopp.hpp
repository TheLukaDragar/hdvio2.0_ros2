/*********************************************************************************
 *  HDVIO++ - Visual Inertial Odometry ++
 *  Code based on:
 *  OKVIS - Open Keyframe-based Visual-Inertial SLAM
 *  Copyright (c) 2015, Autonomous Systems Lab / ETH Zurich
 *  Copyright (c) 2016, ETH Zurich, Wyss Zurich, Zurich Eye
 *  Copyright (c) 2024, Robotics and Perception Group, University of Zurich
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
 *  Created on: Apr 29, 2024
 *      Author: Giovanni Cioffi (cioffi@ifi.uzh.ch)
 *********************************************************************************/

/**
 * @file DynamicsErrorHDVIOpp.hpp
 * @brief Header file for the DynamicsErrorHDVIOpp class.
 * @author Giovanni Cioffi
 */

#pragma once

#include <mutex>

#pragma diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// Eigen 3.2.7 uses std::binder1st and std::binder2nd which are deprecated since
// c++11.
// Fix is in 3.3 devel (http://eigen.tuxfamily.org/bz/show_bug.cgi?id=872).
#include <ceres/ceres.h>
#pragma diagnostic pop

#include <svo/dynamics_handler.h>
#include <svo/common/types.h>
#include <svo/vio_common/logging.hpp>

#include "svo/ceres_backend/estimator_types.hpp"
#include "svo/ceres_backend/error_interface.hpp"


namespace svo {
namespace ceres_backend {

// to make things a bit faster than using angle-axis conversion:
__inline__ double dynhdviopp_sinc(double x)
{
  if (fabs(x) > 1e-6)
  {
   return sin(x) / x;
  }
  else
  {
    static const double c_2 = 1.0 / 6.0;
    static const double c_4 = 1.0 / 120.0;
    static const double c_6 = 1.0 / 5040.0;
    const double x_2 = x * x;
    const double x_4 = x_2 * x_2;
    const double x_6 = x_2 * x_2 * x_2;
    return 1.0 - c_2 * x_2 + c_4 * x_4 - c_6 * x_6;
  }
}

__inline__ Eigen::Quaterniond dynhdviopp_deltaQ(const Eigen::Vector3d& dAlpha)
{
  Eigen::Vector4d dq;
  double halfnorm = 0.5 * dAlpha.template tail<3>().norm();
  dq.template head<3>() = dynhdviopp_sinc(halfnorm) * 0.5 * dAlpha.template tail<3>();
  dq[3] = cos(halfnorm);
  return Eigen::Quaterniond(dq);
}

/// \brief Implements a nonlinear Dynamics factor.
class DynamicsErrorHDVIOpp :
    public ceres::SizedCostFunction<12 /* number of residuals */,
        7 /* size of first parameter (PoseParameterBlock k) */,
        9 /* size of second parameter (SpeedAndBiasParameterBlock k) */,
        7 /* size of third parameter (PoseParameterBlock k+1) */,
        9 /* size of fourth parameter (SpeedAndBiasParameterBlock k+1) */,
        3 /* size of fifth parameter (ExternalForceParameterBlock k+1) */>,
    public ErrorInterface
{
 public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /// \brief The base in ceres we derive from
  typedef ceres::SizedCostFunction<12, 7, 9, 7, 9, 3> base_t;

  /// \brief The number of residuals
  /// pose (6), velocity (3), ext force (3)
  static const int kNumResiduals = 12;

  /// \brief The type of the covariance.
  typedef Eigen::Matrix<double, 15, 15> covariance_t;

  /// \brief The type of the information (same matrix dimension as covariance).
  typedef covariance_t information_t;

  /// \brief Default constructor -- assumes information recomputation.
  DynamicsErrorHDVIOpp() = default;

  /// \brief Trivial destructor.
  virtual ~DynamicsErrorHDVIOpp() = default;

  /// \brief Construct with measurements and parameters.
  /// \@param[in] dynamics_measurements The Dynamics measurements including timestamp
  /// \@param[in] dynamics_params The parameters to be used.
  /// \@param[in] t_0 Start time.
  /// \@param[in] t_1 End time.
  DynamicsErrorHDVIOpp(
    const DynamicsMeasurements &dynamics_measurements,
    const DynamicsHandlerOptions& dynamics_params, const double &t_0, const double &t_1);

  /**
   * @brief Propagates pose and speed with given dynamics measurements.
   * @warning This is not actually const, since the re-propagation must somehow
   *          be stored...
   * @param[in] T_WS Start pose.
   * @param[in] speed_and_biases Start speed and biases (gyro bias isn't used).
   * @return Number of integration steps.
   */
  int redoPreintegration(const Transformation& T_WS,
                         const SpeedAndBias& speed_and_biases) const;

  // setters
  void setRedo(const bool redo = true) const
  {
    redo_ = redo;
  }

  /// \brief (Re)set the parameters.
  /// \@param[in] dynamicsParameters The parameters to be used.
  void setDynamicsParameters(const DynamicsHandlerOptions& dynamics_parameters)
  {
    dynamics_parameters_ = dynamics_parameters;
  }

  /// \brief (Re)set the measurements
  void setDynamicsMeasurements(const DynamicsMeasurements& dynamics_measurements)
  {
    dynamics_measurements_ = dynamics_measurements;
  }

  /// \brief (Re)set the start time.
  /// \@param[in] t_0 Start time.
  void setT0(const double& t_0) { t0_ = t_0; }

  /// \brief (Re)set the start time.
  /// \@param[in] t_1 End time.
  void setT1(const double& t_1) { t1_ = t_1; }

  // getters

  /// \brief Get the dynamics Parameters.
  /// \return the dynamics parameters.
  const DynamicsHandlerOptions& dynamicsParameters() const
  {
    return dynamics_parameters_;
  }

  /// \brief Get the dynamics measurements.
  const DynamicsMeasurements dynamicsMeasurements() const
  {
    return dynamics_measurements_;
  }

  /// \brief Get the start time.
  double t0() const { return t0_; }

  /// \brief Get the end time.
  double t1() const { return t1_; }

  /// \brief Integrate (acc - thr) to ge a prior on the external force
  const int integrateExternalForcePrior(
    const SpeedAndBias& speed_and_biases,
    ExternalForce& ext_force_prior);

  // error term and Jacobian implementation
  /**
   * @brief This evaluates the error term and additionally computes the Jacobians.
   * @param parameters Pointer to the parameters (see ceres)
   * @param residuals Pointer to the residual vector (see ceres)
   * @param jacobians Pointer to the Jacobians (see ceres)
   * @return success of th evaluation.
   */
  virtual bool Evaluate(double const* const * parameters, double* residuals,
                        double** jacobians) const;

  /**
   * @brief This evaluates the error term and additionally computes
   *        the Jacobians in the minimal internal representation.
   * @param parameters Pointer to the parameters (see ceres)
   * @param residuals Pointer to the residual vector (see ceres)
   * @param jacobians Pointer to the Jacobians (see ceres)
   * @param jacobians_minimal Pointer to the minimal Jacobians
   *        (equivalent to jacobians).
   * @return Success of the evaluation.
   */
  bool EvaluateWithMinimalJacobians(double const* const * parameters,
                                    double* residuals, double** jacobians,
                                    double** jacobians_minimal) const;

  // sizes
  /// \brief Residual dimension.
  size_t residualDim() const
  {
    return kNumResiduals;
  }

  /// \brief Number of parameter blocks.
  virtual size_t parameterBlocks() const
  {
    return parameter_block_sizes().size();
  }

  /// \brief Dimension of an individual parameter block.
  /// @param[in] parameter_block_idx Index of the parameter block of interest.
  /// \return The dimension.
  size_t parameterBlockDim(size_t parameter_block_idx) const
  {
    return base_t::parameter_block_sizes().at(parameter_block_idx);
  }

  /// @brief Return parameter block type as string
  virtual ErrorType typeInfo() const
  {
    return ErrorType::kDynamicsError;
  }

 protected:
  // parameters
  DynamicsHandlerOptions dynamics_parameters_; ///< The dynamics parameters.

  // measurements
  DynamicsMeasurements dynamics_measurements_;

  // times
  double t0_; ///< The start time (i.e. time of the first set of states).
  double t1_; ///< The end time (i.e. time of the sedond set of states).

  // preintegration stuff. the mutable is a TERRIBLE HACK, but what can I do.
  mutable std::mutex preintegration_mutex_;
  ///< Protect access of intermediate results.

  // increments (initialise with identity)
  mutable Eigen::Quaterniond Delta_q_ = Eigen::Quaterniond(1,0,0,0);
  ///< Intermediate result
  mutable Eigen::Matrix3d C_integral_ = Eigen::Matrix3d::Zero();
  ///< Intermediate result
  mutable Eigen::Matrix3d C_doubleintegral_ = Eigen::Matrix3d::Zero();
  ///< Intermediate result
  mutable Eigen::Vector3d thr_integral_ = Eigen::Vector3d::Zero();
  ///< Intermediate result
  mutable Eigen::Vector3d thr_doubleintegral_ = Eigen::Vector3d::Zero();
  ///< Intermediate result
  mutable Eigen::Vector3d ext_force_integral_ = Eigen::Vector3d::Zero();

  /// \brief The Jacobian of the increment (w/o biases).
  /// error state includes ext force as well:
  /// d_z = [d_alpha, d_theta, d_beta, d_ba, d_extForce]
  mutable Eigen::Matrix<double,15,15> P_delta_ =
      Eigen::Matrix<double,15,15>::Zero();

  /// \brief Reference gyroscope bias that is updated when called redoPreintegration.
  mutable SpeedAndBias speed_and_biases_ref_ = SpeedAndBias::Zero();

  mutable bool redo_ = true;
  ///< Keeps track of whether or not this redoPreintegration() needs to be done.
  mutable int redoCounter_ = 0;
  ///< Counts the number of preintegrations for statistics.

  // information matrix and its square root
  mutable information_t information_;
  ///< The information matrix for this error term.
  mutable information_t square_root_information_;
  ///< The square root information matrix for this error term.

};

}  // namespace ceres_backend
}  // namespace svo
