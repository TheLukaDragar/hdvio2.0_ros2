#include "svo/ceres_backend_interface.hpp"

#include <svo/common/conversions.h>
#include <svo/common/frame.h>
#include <svo/map.h>
#include <svo/imu_handler.h>
#include <svo/global.h>
#include <fstream>

#include "svo/motion_detector.hpp"
#include "svo/outlier_rejection.hpp"

//! @todo Esimation of extrinsics not tested!
DEFINE_double(extrinsics_sigma_rel_translation, 0.0,
              "Relative translation sigma (temporal) of camera extrinsics");
DEFINE_double(extrinsics_sigma_rel_orientation, 0.0,
              "Relative translation sigma (temporal) of camera extrinsics");

namespace svo
{
CeresBackendInterface::CeresBackendInterface(
    const CeresBackendInterfaceOptions& options,
    const CeresBackendOptions& optimizer_options,
    const MotionDetectorOptions& motion_detector_options,
    const CameraBundlePtr& camera_bundle)
  : options_(options), optimizer_options_(optimizer_options)
{
  type_ = BundleAdjustmentType::kCeres;

  // Setup modules
  if (options_.use_zero_motion_detection)
  {
    motion_detector_.reset(new MotionDetector(motion_detector_options));
  }
  if (options_.use_outlier_rejection)
  {
    outlier_rejection_.reset(
        new OutlierRejection(options_.outlier_rejection_px_threshold));
  }
  // Cameras -------------------------------------------------------------------
  // For now do not estimate extrinsics. (NOT TESTED!)
  if (options.refine_extrinsics)
  {
    ExtrinsicsEstimationParametersVec extrinsics_estimation_parameters(
        camera_bundle->getNumCameras(),
        ExtrinsicsEstimationParameters(options.extrinsics_pos_sigma_meter,
                                       options.extrinsics_rot_sigma_rad,
                                       FLAGS_extrinsics_sigma_rel_translation,
                                       FLAGS_extrinsics_sigma_rel_orientation));
    backend_.addCameraBundle(extrinsics_estimation_parameters, camera_bundle);
  }
  else
  {
    ExtrinsicsEstimationParametersVec extrinsics_estimation_parameters(
        camera_bundle->getNumCameras(),
        ExtrinsicsEstimationParameters(0.0, 0.0, 0.0, 0.0));
    backend_.addCameraBundle(extrinsics_estimation_parameters, camera_bundle);
  }

  // Soft time limit for backend
  if (optimizer_options_.max_iteration_time > 0.0)
  {
    backend_.setOptimizationTimeLimit(optimizer_options_.max_iteration_time, 1);
  }

  backend_.min_num_3d_points_for_fixation_ =
      optimizer_options_.remove_fixation_min_num_fixed_landmarks_;
}

CeresBackendInterface::~CeresBackendInterface()
{
  if (thread_ != nullptr)
  {
    quitThread();
  }
}

// Get a motion prior for new_frames and update the frontend map and last_frames
// (note that map is not used, but actually all keyframes in map are updated
//  in call to updateActiveKeyframes() )
void CeresBackendInterface::loadMapFromBundleAdjustment(
    const FrameBundlePtr& new_frames, const FrameBundlePtr& last_frames,
    const Map::Ptr& map, bool& have_motion_prior)
{
  if (stop_thread_)
  {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_backend_);

  // Setup motion detector ----------------------------------------------------
  if (motion_detector_)
  {
    motion_detector_->setFrames(last_frames, new_frames);
  }

  // Adding new state to backend ---------------------------------------------
  if (addStatesAndInertialMeasurementsToBackend(new_frames))
  {
    last_added_nframe_imu_ = new_frames->getBundleId();

    // Obtain motion prior ---------------------------------------------------
    updateBundleStateWithBackend(new_frames, true);
    have_motion_prior = true;
  }
  else
  {
    LOG(ERROR) << "Could not add frame bundle " << new_frames->getBundleId()
               << " to backend";
    have_motion_prior = false;
  }

  if (imu_handler_ && imu_handler_->options_.temporal_stationary_check)
  {
    IMUTemporalStatus imu_status =
        imu_handler_->checkTemporalStatus(new_frames->at(0)->getTimestampSec());
    imu_motion_detector_stationary_ =
        (imu_status == IMUTemporalStatus::kStationary);
  }

  // Update Frames and Map ---------------------------------------------------
  if (last_updated_nframe_ == last_optimized_nframe_.load())
  {
    VLOG(3) << "VIN: No map update available.";
    return;
  }

  // Update SVO Map ----------------------------------------------------------
  {
    Transformation T_WS;
    // Statistics
    int n_frames_updated = 0;

    VLOG(3) << "Updating states with latest results from ceres optimizer.";
    //! @todo this is not very efficient for multiple cameras,
    //! because we update each frame separately
    //! this we we need to get T_WS twice
    //! @todo store framebundles in map to solve problem
    for (FramePtr& keyframe : active_keyframes_)
    {
      DEBUG_CHECK(keyframe) << "Found nullptr keyframe";
      updateFrameStateWithBackend(keyframe, false);
      n_frames_updated++;
    }
    VLOG(3) << "Updated " << n_frames_updated << " frames in map.";

    // Update the 3d points in map of the updated keyframes ----------------
    // Statistics
    backend_.updateAllActivePoints();
  }

  // Update last frame bundle ------------------------------------------------
  {
    // Last frames might not be keyframes (are not yet updated => update pose)
    for (FramePtr& last_frame : *last_frames)
    {
      if (!last_frame->isKeyframe())
      {
        updateFrameStateWithBackend(last_frame, true);
      }
    }

    // Remove outliers of last_frames ----------------------------------------
    if (outlier_rejection_)
    {
      if (last_frames)
      {
        size_t n_deleted_edges = 0;
        size_t n_deleted_corners = 0;
        std::vector<int> deleted_points;
        for (FramePtr& frame : *last_frames)
        {
          outlier_rejection_->removeOutliers(*frame, n_deleted_edges,
                                             n_deleted_corners, deleted_points,
                                             !lock_to_fixed_landmarks_);
        }
        //! @todo should we only remove observation but leave points?
        backend_.removePointsByPointIds(deleted_points);
        VLOG(6) << "Outlier rejection: removed " << n_deleted_edges
                << " edgelets and " << n_deleted_corners << " corners.";
      }
    }

    // The following is not used for the algorithm to work, but updated for
    // completeness.
    SpeedAndBias speed_and_bias;
    bool success =
        backend_.getSpeedAndBias(last_frames->getBundleId(), speed_and_bias);
    DEBUG_CHECK(success) << "Could not get speed and bias estimate from ceres "
                            "optimizer";
    imu_handler_->setAccelerometerBias(speed_and_bias.tail<3>());
    imu_handler_->setGyroscopeBias(speed_and_bias.segment<3>(3));

    publisher_->addFrame(last_added_nframe_imu_);
  }

  // shift state
  last_updated_nframe_ = last_optimized_nframe_.load();
}

// Add feature correspondences and landmarks to backend
void CeresBackendInterface::bundleAdjustment(const FrameBundlePtr& frame_bundle)
{
  if (stop_thread_)
  {
    return;
  }

  // check for case when IMU measurements could not be added.
  if (last_added_nframe_imu_ == last_added_nframe_images_)
  {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_backend_);

  vk::Timer timer;
  timer.start();
  // Checking for zero motion ------------------------------------------------
  bool velocity_prior_added = false;
  if (motion_detector_)
  {
    double sigma = 0;
    if (!motion_detector_->isImageMoving(sigma))
    {
      ++no_motion_counter_;

      if (no_motion_counter_ > options_.backend_zero_motion_check_n_frames)
      {
        image_motion_detector_stationary_ = true;
        VLOG(5) << "Image is not moving: adding zero velocity prior.";
        if (!backend_.addVelocityPrior(
                createNFrameId(frame_bundle->getBundleId()),
                Eigen::Matrix<FloatType, 3, 1>::Zero(), sigma))
        {
          LOG(ERROR) << "Failed to add a zero velocity prior!";
          DEBUG_CHECK(false) << "Not able to add velocity prior";
        }
        else
        {
          velocity_prior_added = true;
        }
      }
    }
    else
    {
      image_motion_detector_stationary_ = false;
      no_motion_counter_ = 0;
    }
  }

  // only use imu-based motion detection when the images are not good
  if (!image_motion_detector_stationary_ && imu_motion_detector_stationary_)
  {
    VLOG(5) << "IMU determined stationary, adding prior at time "
            << frame_bundle->at(0)->getTimestampSec() << std::endl;
    if (!backend_.addVelocityPrior(createNFrameId(frame_bundle->getBundleId()),
                                   Eigen::Matrix<FloatType, 3, 1>::Zero(),
                                   0.005))
    {
      LOG(ERROR) << "Failed to add a zero velocity prior!";
      DEBUG_CHECK(false) << "Not able to add velocity prior";
    }
    else
    {
      velocity_prior_added = true;
    }
  }

  // Adding new landmarks to backend -----------------------------------------
  size_t num_new_observations = 0;
  for (FramePtr& frame : *frame_bundle)
  {
    if (frame->isKeyframe())
    {
      backend_.setKeyframe(createNFrameId(frame->bundleId()), true);
      active_keyframes_.push_back(frame);
      addLandmarksAndObservationsToBackend(frame);
    }
    else
    {
      // add observations for landmarks that are still visible
      for (size_t kp_idx = 0; kp_idx < frame->numFeatures(); ++kp_idx)
      {
        if (frame->landmark_vec_[kp_idx] &&
            backend_.isPointInEstimator(frame->landmark_vec_[kp_idx]->id()))
        {
          if (backend_.addObservation(frame, kp_idx))
          {
            ++num_new_observations;
          }
        }
      }
    }
  }
  VLOG(10) << "Backend: Added " << num_new_observations
           << " continued observation in non-KF to backend.";

  if (options_.skip_optimization_when_tracking_bad)
  {
    if (frame_bundle->numLandmarksInBA() < options_.min_added_measurements)
    {
      LOG(WARNING) << "Too few visual measurements, skip optimization once.";
      skip_optimization_once_ = true;
    }
  }

  if (velocity_prior_added)
  {
    LOG(WARNING) << "Velocity prior added, not skipping optimization.";
    skip_optimization_once_ = false;
  }

  // @ todo: This probably needs the global map. If so, it's not supported
  if (global_landmark_value_version_ < Point::global_map_value_version_)
  {
    backend_.updateFixedLandmarks();
    VLOG(1) << "Update fixed landmarks in Ceres backend: "
            << global_landmark_value_version_ << " ==> "
            << Point::global_map_value_version_ << std::endl;
    global_landmark_value_version_ = Point::global_map_value_version_;
  }

  last_added_nframe_images_ = frame_bundle->getBundleId();
  // it will be 0 for the first frame.
  before_the_last_added_frame_stamp_ns_ = last_added_frame_stamp_ns_;
  last_added_frame_stamp_ns_ = frame_bundle->getMinTimestampNanoseconds();
  if (g_permon_backend_)
  {
    g_permon_backend_->log("pre_optim_time", timer.stop());
  }
  wait_condition_.notify_one();
}

// Add all landmarks and observations of frame (under certain criteria)
void CeresBackendInterface::addLandmarksAndObservationsToBackend(
    const FramePtr& frame)
{
  // Statistics.
  size_t n_skipped_points_parallax = 0;
  size_t n_skipped_few_obs = 0;
  size_t n_features_already_in_backend = 0;
  size_t n_new_observations = 0;
  size_t n_new_landmarks = 0;
  size_t n_skipped_not_corner = 0;

  std::vector<std::pair<size_t, size_t>> kp_idx_to_n_obs_map_fixed_lm;

  // iterate through all features
  for (size_t kp_idx = 0; kp_idx < frame->numFeatures(); ++kp_idx)
  {
    const PointPtr& point = frame->landmark_vec_[kp_idx];
    const FeatureType& type = frame->type_vec_[kp_idx];

    // check if feature is associated to landmark
    if (point == nullptr)
    {
      continue;
    }

    // check if landmark was already in to backend, if yes just add observation.
    if (backend_.isPointInEstimator(point->id()))
    {
      ++n_features_already_in_backend;
      if (!backend_.addObservation(frame, kp_idx))
      {
        LOG(WARNING) << "Failed to add an observation!";
        continue;
      }
      ++n_new_observations;
    }
    else
    {
      if (isMapPoint(frame->type_vec_[kp_idx]))
      {
        continue;
      }
      if (options_.only_use_corners)
      {
        if (frame->type_vec_[kp_idx] != FeatureType::kCorner ||
            frame->type_vec_[kp_idx] != FeatureType::kFixedLandmark)
        {
          ++n_skipped_not_corner;
          continue;
        }
      }

      // check if we have enough observations. Might not be the case if seed
      // original frame was already dropped.
      if (point->obs_.size() < options_.min_num_obs)
      {
        VLOG(10) << "Point with less than " << options_.min_num_obs
                 << " observations! Only have " << point->obs_.size();
        ++n_skipped_few_obs;
        continue;
      }

      DEBUG_CHECK(!std::isnan(point->pos_[0])) << "Point is nan!";

      //      //! @todo tune this parameter, do we need it?
      //      if(point->getTriangulationParallax() <
      //      options_.min_parallax_thresh)
      //      {
      //        ++n_skipped_points_parallax;
      //        continue;
      //      }

      //! @todo We should first get all candidate points and sort them
      //!   according to parallax angle and num observations. afterwards only
      //!   add best N observations.
      // add the landmark
      if (isFixedLandmark(type))
      {
        kp_idx_to_n_obs_map_fixed_lm.emplace_back(
            std::make_pair(kp_idx, point->obs_.size()));
        continue;
      }
      if (!backend_.addLandmark(point, false))
      {
        LOG(ERROR) << "Failed to add a landmark!";
        continue;
      }
      ++n_new_landmarks;
      // add an observation to the landmark
      if (!backend_.addObservation(frame, kp_idx))
      {
        LOG(ERROR) << "Failed to add an observation!";
        continue;
      }
      ++n_new_observations;
    }
  }  // landmarks

  // for fixed landmarks
  std::sort(kp_idx_to_n_obs_map_fixed_lm.begin(),
            kp_idx_to_n_obs_map_fixed_lm.end(),
            [](const std::pair<size_t, size_t>& p1,
               const std::pair<size_t, size_t>& p2) {
              return p1.second > p2.second;
            });

  size_t n_added_fixed_lm = 0;
  for (size_t idx = 0; idx < kp_idx_to_n_obs_map_fixed_lm.size(); idx++)
  {
    const size_t cur_kp_idx = kp_idx_to_n_obs_map_fixed_lm[idx].first;
    backend_.addLandmark(frame->landmark_vec_[cur_kp_idx], true);
    backend_.addObservation(frame, cur_kp_idx);
    n_added_fixed_lm++;
    if (backend_.numFixedLandmarks() >=
        optimizer_options_.max_fixed_lm_in_ceres_)
    {
      break;
    }
  }

  VLOG(6) << "Backend has: " << backend_.numFixedLandmarks()
          << " fixed landmarks out of " << backend_.numLandmarks() << std::endl;
  VLOG(6) << "Backend: Added " << n_new_landmarks << " new landmarks";
  VLOG(6) << "Backend: Added " << n_new_observations << " new observations";
  VLOG(6) << "Backend: Observations already in backend: "
          << n_features_already_in_backend;
  VLOG(6) << "Backend: Adding points. Skipped because less than "
          << options_.min_num_obs << " observations: " << n_skipped_few_obs;
  VLOG(6) << "Backend: Adding points. Skipped because small parallax: "
          << n_skipped_points_parallax;
  VLOG(6) << "Backend: Adding points. Skipped because not corner: "
          << n_skipped_not_corner;
}

// Introduce a state for the frame_bundle in backend. Add IMU terms.
bool CeresBackendInterface::addStatesAndInertialMeasurementsToBackend(
    const FrameBundlePtr& frame_bundle)
{
  // Gather required IMU measurements ----------------------------------------
  ImuMeasurements imu_measurements;
  const double current_frame_bundle_stamp = frame_bundle->getMinTimestampSeconds();
  if (!imu_handler_->waitTill(current_frame_bundle_stamp))
  {
    return false;
  }

  // Get measurements, newest is interpolated to exactly match timestamp of
  // frame_bundle
  if (!imu_handler_->getMeasurementsContainingEdges(
    current_frame_bundle_stamp, imu_measurements, true))
  {
    LOG(ERROR) << "Could not retrieve IMU measurements."
               << " Last frame was at " << last_added_frame_stamp_ns_
               << ", current is at "
               << frame_bundle->getMinTimestampNanoseconds();
    return false;
  }

  // Gather dynamics measurements ----------------------------------------
  DynamicsMeasurements dynamics_measurements;
  bool add_dynamics_factor = false;
  if ((dynamics_handler_->options_.use_dynamics) && 
  (frame_bundle->getBundleId() > dynamics_handler_->options_.n_init_frames))
  {
    add_dynamics_factor = true;

    if (dynamics_handler_->options_.use_rotational_dynamics)
    {
      // we need to initialize/update knots with the new ang. vels.
      // compute the timestamps of the new knots
      // This has to happen in any case, even if the dynamic factor won't be added to the backend
      double t0;
      double dt = opt_quad_angvel_spline_->get_dt_s();
      size_t n_knots = opt_quad_angvel_spline_->get_num_knots();
      if (n_knots == 0)
      {
        t0 = imu_measurements.back().timestamp_;
      }
      else
      {
        t0 = opt_quad_angvel_spline_->get_start_time_s() + (static_cast<double>(n_knots)-1) * dt;
      }

      double t = t0;
      std::deque<double> knot_ts;
      // we will copy the values of the newest/oldest value in case interpolation can't be computed.
      size_t non_interp_cnt_back = 0;
      size_t non_interp_cnt_front = 0;
      while (t < imu_measurements.front().timestamp_)
      {
        t += dt;

        if (t < imu_measurements.back().timestamp_)
        {
          non_interp_cnt_back++;
        }
        else if (t > imu_measurements.front().timestamp_)
        {
          non_interp_cnt_front++;
        }
        else
        {
          knot_ts.push_back(t);
        }
      }

      if (dynamics_handler_->options_.debug_info)
      {
        std::cout << "=== Rotational Dynamis Debug INFO ===\n";
        std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> curr_knots;
        std::deque<int64_t> curr_knots_times;
        opt_quad_angvel_spline_->get_knots(curr_knots);
        opt_quad_angvel_spline_->get_knots_times(curr_knots_times);
        std::cout << "current knots\n";
        for (size_t i = 0; i < curr_knots.size(); i++)
        {
          std::cout << "knot[" << i << "], time = " << std::setprecision(15) 
          << opt_quad_angvel_spline_->nsec_to_sec(curr_knots_times[i]) 
          << ", val = " << curr_knots[i](0) << ", " << curr_knots[i](1) << ", " << curr_knots[i](2) << "\n";
        }
        std::cout << "\n";

        std::cout << "imu_measurements.size() = " << imu_measurements.size() << "\n";
        std::cout << "imu_measurements.back().timestamp_ = " << std::setprecision(15) << imu_measurements.back().timestamp_ << "\n";
        std::cout << "imu_measurements.front().timestamp_ = " << std::setprecision(15) << imu_measurements.front().timestamp_ << "\n";
        std::cout << "\n";

        std::cout << "new knot_ts\n";
        for (size_t i = 0; i < knot_ts.size(); i++)
        {
          std::cout << "new knot_ts[" << i << "] = " << std::setprecision(15) << knot_ts[i] << "\n";
        }
        std::cout << "\n";
        std::cout << "non_interp_cnt_back = " << non_interp_cnt_back << "\n";
        std::cout << "non_interp_cnt_front = " << non_interp_cnt_front << "\n";
      }
      
      // if ((knot_ts.size() == 0) && (non_interp_cnt_back == 0) && (non_interp_cnt_front == 0))
      if (knot_ts.size() == 0)
      {
        add_dynamics_factor = false;
      }
      else
      {
        std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> knot_vals;
        imu_handler_->interpolate_ang_vels(imu_measurements, knot_ts, knot_vals);

        // deal with time not interpolated
        for (size_t i = 0; i < non_interp_cnt_front; i++)
        {
          knot_ts.push_back(knot_ts.back()+dt*(i+1));
          knot_vals.push_back(knot_vals.back());
        }

        for (size_t i = 0; i < non_interp_cnt_back; i++)
        {
          knot_ts.push_front(knot_ts.front()-dt*(i+1));
          knot_vals.push_front(knot_vals.front());
        }

        if (dynamics_handler_->options_.debug_info)
        {
          std::cout << "=== Rotational Dynamis Debug INFO ===\n";
          std::cout << "interpolated knots\n";
          for (size_t i = 0; i < knot_vals.size(); i++)
          {
            std::cout << "knot[" << i << "], time = " << std::setprecision(15) 
            << knot_ts[i]
            << ", val = " << knot_vals[i](0) << ", " << knot_vals[i](1) << ", " << knot_vals[i](2) << "\n";
          }
          std::cout << "\n";
        }

        // Update knots
        opt_quad_angvel_spline_->set_knots(
          knot_vals, opt_quad_angvel_spline_->sec_to_nsec(knot_ts.at(0)));

        // we haven't had enough knots yet
        int n_knots_now = static_cast<int>(opt_quad_angvel_spline_->get_num_knots());
        if (n_knots_now < opt_quad_angvel_spline_->get_degree())
        {
          add_dynamics_factor = false;
        }
      }

      if (dynamics_handler_->options_.debug_info)
      {
        std::cout << "=== Rotational Dynamis Debug INFO ===\n";
        std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> updated_knots;
        std::deque<int64_t> updated_knots_times;
        opt_quad_angvel_spline_->get_knots(updated_knots);
        opt_quad_angvel_spline_->get_knots_times(updated_knots_times);
        std::cout << "updated knots\n";
        for (size_t i = 0; i < updated_knots.size(); i++)
        {
          std::cout << "knot[" << i << "], time = " << std::setprecision(15) 
          << opt_quad_angvel_spline_->nsec_to_sec(updated_knots_times[i]) 
          << ", val = " << updated_knots[i](0) << ", " << updated_knots[i](1) << ", " << updated_knots[i](2) << "\n";
        }
        std::cout << "\n";
      }
    }

    // Get dynamics measurements
    if (!dynamics_handler_->getMeasurementsExcludingEdges(imu_measurements.back().timestamp_,
      imu_measurements.front().timestamp_, dynamics_measurements, true))
    {
      VLOG(0) << "Could not retrieve dynamics measurements." 
      << " Last frame was at " << last_added_frame_stamp_ns_ 
      << ", current is at " << frame_bundle->getMinTimestampNanoseconds();
      add_dynamics_factor = false;
    }

    if (dynamics_handler_->options_.use_rotational_dynamics)
    {
      // Sample accelerations to get the prior for the external force mean
      // Sample gyro for the net input
      if (!dynamics_handler_->sampleImuMeasurements(imu_measurements, dynamics_measurements))
      {
        VLOG(0) << "Could not interpolate imu measurements at dynamics time.";
        add_dynamics_factor = false;
      }
    }

  }

  // Collect dynamics data
  if (add_dynamics_factor)
  {
    if (dynamics_handler_->options_.use_rotational_dynamics)
    {
      // add rotational dynamics measurements
      double t0_s = opt_quad_angvel_spline_->min_time_s();
      double t1_s = opt_quad_angvel_spline_->max_time_s() - opt_quad_angvel_spline_->get_dt_s()*0.01;

      if ((dynamics_handler_->options_.mode == 2) || (dynamics_handler_->options_.mode == 3))
      {
        std::vector<double> torque_meas_ts_s;
        std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> torque_meas;
        // check measurements from current window
        for (int j = dynamics_measurements.size()-1; j >=0 ; j--)
        {
          torque_meas_ts_s.push_back(dynamics_measurements.at(j).timestamp_);
          torque_meas.push_back(dynamics_measurements.at(j).collective_torque_);
        }
        
        // add learned torque residual
        if (dynamics_handler_->options_.use_learned_residuals)
        {
          // add measurements to net input buffer
          dynamics_handler_->addNetInputs(dynamics_measurements);

          // run network if buffer is ready
          std::deque<DynamicsMeasurement> net_inputs;
          if (dynamics_handler_->getNetInputs(net_inputs, "torque"))
          {
            int net_input_size = dynamics_handler_->getTorqueNetInBufferSize();
            float* inputs = nullptr;
            inputs = new float[6*net_input_size];

            size_t i = 0;
            for (auto it=net_inputs.begin(); it!=net_inputs.end(); ++it)
            {
              inputs[i] = it->gyro_[0];
              inputs[i+net_input_size] = it->gyro_[1];
              inputs[i+2*net_input_size] = it->gyro_[2];
              inputs[i+3*net_input_size] = it->collective_torque_[0];
              inputs[i+4*net_input_size] = it->collective_torque_[1];
              inputs[i+5*net_input_size] = it->collective_torque_[2];

              i++;
            }

            // logging
            const int log_map_verbose_level_in = 100;
            if ((FLAGS_v >= log_map_verbose_level_in))
            {
              VLOG(log_map_verbose_level_in) << "Dynamic net input buffer:";
              VLOG(log_map_verbose_level_in) << "size: " << net_inputs.size();
              for (int j = 0; j < net_input_size; j++)
              {
                std::string s;
                const double t = net_inputs.at(j).timestamp_;
                const Eigen::Vector3d w(
                  inputs[j], inputs[j+net_input_size], inputs[j+2*net_input_size]
                );
                const Eigen::Vector3d tor(
                  inputs[j+3*net_input_size], inputs[j+4*net_input_size], inputs[j+5*net_input_size]
                );
                s = "\nts= " + std::to_string(t) +
                "\ntorque= [" + std::to_string(tor.x()) + ", " + std::to_string(tor.y()) + ", " + std::to_string(tor.z()) +
                "]\nomega= [" + std::to_string(w.x()) + ", " + std::to_string(w.y()) + ", " + std::to_string(w.z()) + "]\n";
                VLOG(log_map_verbose_level_in) << s;
              }
            }

            // run inference
            float* output = nullptr;
            output = torque_dynamics_net_->inference(inputs);

            Eigen::Vector3d res_torque(output[0], output[1], output[2]);

            // logging
            const int log_map_verbose_level_out = 100;
            if ((FLAGS_v >= log_map_verbose_level_out))
            {
              VLOG(log_map_verbose_level_out) << "Dynamic net output";
              std::string s;
              s = "residual torque= [" + std::to_string(res_torque.x()) + 
              ", " + std::to_string(res_torque.y()) + ", " + std::to_string(res_torque.z()) + "]\n";
              VLOG(log_map_verbose_level_out) << s;
            }

            if (res_torque.norm() > 1.0)
            {
              SVO_WARN_STREAM("Network correction rejected! Res torque norm = " << res_torque.norm());
              res_torque = Eigen::Vector3d::Zero();
            }

            // apply residual
            for (auto it=dynamics_measurements.begin(); it!=dynamics_measurements.end(); ++it)
            {
              it->collective_torque_ += res_torque;
            }

          }
        }

        opt_quad_angvel_spline_->add_rot_dynamics_measurements(torque_meas_ts_s, torque_meas);

        if (dynamics_handler_->options_.debug_info)
        {
          std::cout << "=== Rotational Dynamis Debug INFO ===\n";
          std::cout << "torque_meas\n";
          for (size_t i = 0; i < torque_meas.size(); i++)
          {
            std::cout << "torque_meas[" << i << "], time = " << std::setprecision(15) 
            << torque_meas_ts_s[i]
            << ", val = " << torque_meas[i](0) << ", " << torque_meas[i](1) << ", " << torque_meas[i](2) << "\n";
          }
          std::cout << "\n";
          std::cout << "dynamics_measurements.size() = " << dynamics_measurements.size() << "\n";
        }
      }

      if (dynamics_handler_->options_.debug_info)
      {
        std::cout << "=== Rotational Dynamis Debug INFO ===\n";
        std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> before_knots;
        std::deque<int64_t> before_knots_times;
        opt_quad_angvel_spline_->get_knots(before_knots);
        opt_quad_angvel_spline_->get_knots_times(before_knots_times);
        std::cout << "Before optimization knots\n";
        for (size_t i = 0; i < before_knots.size(); i++)
        {
          std::cout << "knot[" << i << "], time = " << std::setprecision(15) 
          << opt_quad_angvel_spline_->nsec_to_sec(before_knots_times[i]) 
          << ", val = " << before_knots[i](0) << ", " << before_knots[i](1) << ", " << before_knots[i](2) << "\n";
        }
        std::cout << "\n";
      }

      // add ang vel imu measurements 
      if ((dynamics_handler_->options_.mode == 0) || (dynamics_handler_->options_.mode == 2))
      {
        std::vector<double> angvel_meas_ts_s;
        std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> angvel_meas;

        for (int j = imu_measurements.size()-1; j >= 0; j--)
        {
          angvel_meas_ts_s.push_back(imu_measurements.at(j).timestamp_);
          angvel_meas.push_back(imu_measurements.at(j).angular_velocity_);
        }
        opt_quad_angvel_spline_->add_ang_vel_measurements(angvel_meas_ts_s, angvel_meas);

        if (dynamics_handler_->options_.debug_info)
        {
          std::cout << "=== Rotational Dynamis Debug INFO ===\n";
          std::cout << "angvel_meas\n";
          for (size_t i = 0; i < angvel_meas.size(); i++)
          {
            std::cout << "angvel_meas[" << i << "], time = " << std::setprecision(15) 
            << angvel_meas_ts_s[i]
            << ", val = " << angvel_meas[i](0) << ", " << angvel_meas[i](1) << ", " << angvel_meas[i](2) << "\n";
          }
          std::cout << "\n";

          std::cout << "gyro meas\n";
          for (size_t i = 0; i < imu_measurements.size(); i++)
          {
            std::cout << "imu_measurements[" << i << "], time = " << std::setprecision(15) 
            << imu_measurements[i].timestamp_
            << ", val = " << imu_measurements[i].angular_velocity_(0) 
            << ", " << imu_measurements[i].angular_velocity_(1) 
            << ", " << imu_measurements[i].angular_velocity_(2) << "\n";
          }
          std::cout << "\n";
        }
      }

      // optimize
      bool converged = false;
      double error;
      int opt_iter = 0;
      bool print_info = false;

      const size_t n_meas = opt_quad_angvel_spline_->get_num_of_measurements(dynamics_handler_->options_.mode);
      if (n_meas > 4)
      {
        std::chrono::high_resolution_clock::time_point start;
        if (dynamics_handler_->options_.debug_info)
        {
          start = std::chrono::high_resolution_clock::now();
        }

        while ((!converged) && (opt_iter < dynamics_handler_->options_.max_iter))
        {
            converged = opt_quad_angvel_spline_->optimize(
              dynamics_handler_->options_.stop_thresh, error, dynamics_handler_->options_.mode, print_info);
            opt_iter++;
        }

        if (dynamics_handler_->options_.debug_info)
        {
          std::cout << "=== Rotational Dynamis Debug INFO ===\n";
          auto stop = std::chrono::high_resolution_clock::now();
          auto t_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
          double t_elapsed_ms = t_elapsed.count() * 1e-3;

          std::cout << "optimization time: " << t_elapsed_ms << " [ms]\n";
        }
      }

      if (dynamics_handler_->options_.debug_info)
      {
        std::cout << "=== Rotational Dynamis Debug INFO ===\n";

        std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> after_knots;
        std::deque<int64_t> after_knots_times;
        opt_quad_angvel_spline_->get_knots(after_knots);
        opt_quad_angvel_spline_->get_knots_times(after_knots_times);

        std::cout << "After optimization knots\n";
        for (size_t i = 0; i < after_knots.size(); i++)
        {
          std::cout << "knot[" << i << "], time = " << std::setprecision(15) 
          << opt_quad_angvel_spline_->nsec_to_sec(after_knots_times[i]) 
          << ", val = " << after_knots[i](0) << ", " << after_knots[i](1) << ", " << after_knots[i](2) << "\n";
        }
        std::cout << "\n";
      }

      // get measurements at the borders
      if (!dynamics_handler_->duplicateBorders(imu_measurements, dynamics_measurements))
      {
        VLOG(0) << "Could not duplicate borders.";
        add_dynamics_factor = false;
      }

      // sample spline 
      int64_t min_t_sampling = opt_quad_angvel_spline_->min_time_ns();
      int64_t max_t_sampling = opt_quad_angvel_spline_->max_time_ns();
      int64_t dt_knot = opt_quad_angvel_spline_->get_dt_ns();
      bool sampling_successful = true;

      // compute mean and standard deviation of gyroscope measurements
      // we use this values to check that the spline values are reasonable
      double sum = 0;
      double sq_sum = 0;
      for (const auto im : imu_measurements)
      {
        double angvel_norm = im.angular_velocity_.norm();
        sum += angvel_norm;
        sq_sum += angvel_norm*angvel_norm;
      }
      double angvel_mean = sum / imu_measurements.size();
      double angvel_var = sq_sum / imu_measurements.size() - angvel_mean * angvel_mean;
      double angvel_std = std::sqrt(angvel_var);

      double body_rate_upper_thr = angvel_mean + 10.0*angvel_std;
      double body_rate_lower_thr = angvel_mean - 10.0*angvel_std;;

      if (dynamics_handler_->options_.debug_info)
      {
        std::cout << "=== Rotational Dynamis Debug INFO ===\n";
        std::cout << "body_rate_upper_thr = " << body_rate_upper_thr << "\n"; 
        std::cout << "body_rate_lower_thr = " << body_rate_lower_thr << "\n";
      }

      // we interpolate the knots when the time is too recent for
      std::deque<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> knots;
      std::deque<int64_t> knots_times;
      opt_quad_angvel_spline_->get_knots(knots);
      opt_quad_angvel_spline_->get_knots_times(knots_times);
      size_t idx_knot = knots_times.size() - 1;
      int64_t knot_t1 = knots_times.at(idx_knot);
      int64_t knot_t0 = knots_times.at(idx_knot-1);
      int spline_deg = opt_quad_angvel_spline_->get_degree();

      for (auto it=dynamics_measurements.begin(); it!=dynamics_measurements.end(); ++it)
      {
        int64_t t_sampling = static_cast<int64_t>(it->timestamp_*1e9);
        // initialize with large values so that the threshold check will be rejected 
        // if we can't get a value
        Eigen::Vector3d sampled_body_rate = Eigen::Vector3d::Ones()*1000.;
        double sampled_body_rate_norm;

        if ((t_sampling >= min_t_sampling) && (t_sampling < max_t_sampling))
        {
          opt_quad_angvel_spline_->sample(sampled_body_rate, 0, t_sampling);
          sampled_body_rate_norm = sampled_body_rate.norm();

          if ((sampled_body_rate_norm < body_rate_lower_thr) || (sampled_body_rate_norm > body_rate_upper_thr))
          {
            sampling_successful = false;
          }
        }
        // this happens for the most recent datapoints to sample.
        // We interpolate the knots
        else if ((t_sampling >= max_t_sampling) && (t_sampling < knots_times.back()))
        { 
          for (int i = 0; i < spline_deg; i++)
          {
            if ((t_sampling >= knot_t0) && (t_sampling <= knot_t1))
            {
              // interpolate
              double dt = static_cast<double>(t_sampling - knot_t0) / static_cast<double>(knot_t1 - knot_t0);
              sampled_body_rate = knots.at(idx_knot-1) + dt * (knots.at(idx_knot) - knots.at(idx_knot-1));
              sampled_body_rate_norm = sampled_body_rate.norm();
              break;
            }
            else
            {
              idx_knot--;
              knot_t1 = knots_times.at(idx_knot);
              knot_t0 = knots_times.at(idx_knot-1);
            }
          }

          if ((sampled_body_rate_norm < body_rate_lower_thr) || (sampled_body_rate_norm > body_rate_upper_thr))
          {
            sampling_successful = false;
          }
        }
        else
        {
          sampling_successful = false;
          if (dynamics_handler_->options_.debug_info)
          {
            std::cout << "=== Rotational Dynamis Debug INFO ===\n";
            SVO_WARN_STREAM("Spline sampling failed. sampling time " 
            << t_sampling << " [ns], min sampling time " << min_t_sampling 
            << "[ns], max sampling time " << max_t_sampling << " [ns]");
          }
        }

        if (!sampling_successful)
        {
          break;
        }

        it->body_rates_ = sampled_body_rate;
      }

      if (!sampling_successful)
      {
        add_dynamics_factor = false;
      }

      // debug (Don't use dynamics but publish them for debugging. REMEMBER: Need to uncomment another block)
      // if (sampling_successful)
      // {
      //   backend_.setDynamicsMeasurements(dynamics_measurements);
      //   add_dynamics_factor = false;
      // }
      // end
    }
    else
    {
      if (dynamics_handler_->options_.external_force_zero_mean)
      {
        // Use gyro measurements in the formulation of the dynamic factors.
        // Sample the gyro measurements at thrust times
        if (!dynamics_handler_->sampleGyroscopeMeasurements(imu_measurements, dynamics_measurements))
        {
          VLOG(0) << "Could not interpolate gyro measurements at thrust time.";
          add_dynamics_factor = false;
        }
      }
      else
      {
        // sample also accelerations to get the prior for the external force mean
        if (!dynamics_handler_->sampleImuMeasurements(imu_measurements, dynamics_measurements))
        {
          VLOG(0) << "Could not interpolate imu measurements at thrust time.";
          add_dynamics_factor = false;
        }
      }

      // get measurements at the borders
      if (!dynamics_handler_->duplicateBorders(imu_measurements, dynamics_measurements))
      {
        VLOG(0) << "Could not duplicate borders.";
        add_dynamics_factor = false;
      }
    }
  }
  
  if (!add_dynamics_factor)
  {
    dynamics_measurements.clear();
  }
  else
  {
    // add learned residual thrust.
    if (dynamics_handler_->options_.use_learned_residuals)
    {
      // add measurements to net input buffer, if not already done
      if (!dynamics_handler_->options_.use_rotational_dynamics)
      {
        dynamics_handler_->addNetInputs(dynamics_measurements);
      }

      // run network if buffer is ready
      std::deque<DynamicsMeasurement> net_inputs;
      if (dynamics_handler_->getNetInputs(net_inputs, "thrust"))
      {
        int net_input_size = dynamics_handler_->getThrustNetInBufferSize();
        float* inputs = nullptr;
        inputs = new float[6*net_input_size];

        size_t i = 0;
        for (auto it=net_inputs.begin(); it!=net_inputs.end(); ++it)
        {
          inputs[i] = it->gyro_[0];
          inputs[i+net_input_size] = it->gyro_[1];
          inputs[i+2*net_input_size] = it->gyro_[2];
          inputs[i+3*net_input_size] = it->collective_thrust_[0];
          inputs[i+4*net_input_size] = it->collective_thrust_[1];
          inputs[i+5*net_input_size] = it->collective_thrust_[2];

          i++;
        }

        // logging
        const int log_map_verbose_level_in = 100;
        if ((FLAGS_v >= log_map_verbose_level_in))
        {
          VLOG(log_map_verbose_level_in) << "Dynamic net input buffer:";
          VLOG(log_map_verbose_level_in) << "size: " << net_inputs.size();
          for (int j = 0; j < net_input_size; j++)
          {
            std::string s;
            const double t = net_inputs.at(j).timestamp_;
            const Eigen::Vector3d w(
              inputs[j], inputs[j+net_input_size], inputs[j+2*net_input_size]
            );
            const Eigen::Vector3d thr(
              inputs[j+3*net_input_size], inputs[j+4*net_input_size], inputs[j+5*net_input_size]
            );
            s = "\nts= " + std::to_string(t) +
            "\nthrust= [" + std::to_string(thr.x()) + ", " + std::to_string(thr.y()) + ", " + std::to_string(thr.z()) +
            "]\nomega= [" + std::to_string(w.x()) + ", " + std::to_string(w.y()) + ", " + std::to_string(w.z()) + "]\n";
            VLOG(log_map_verbose_level_in) << s;
          }
        }

        // run inference
        float* output = nullptr;
        output = thrust_dynamics_net_->inference(inputs);

        Eigen::Vector3d res_thrust(output[0], output[1], output[2]);

        // logging
        const int log_map_verbose_level_out = 100;
        if ((FLAGS_v >= log_map_verbose_level_out))
        {
          VLOG(log_map_verbose_level_out) << "Dynamic net output";
          std::string s;
          s = "residual thrust= [" + std::to_string(res_thrust.x()) + 
          ", " + std::to_string(res_thrust.y()) + ", " + std::to_string(res_thrust.z()) + "]\n";
          VLOG(log_map_verbose_level_out) << s;
        }

        if (res_thrust.norm() > 5.0 /*[m/s2]*/)
        {
          SVO_WARN_STREAM("Network correction rejected! Res thrust norm = " << res_thrust.norm());
          res_thrust = Eigen::Vector3d::Zero();
        }

        // apply residual
        for (auto it=dynamics_measurements.begin(); it!=dynamics_measurements.end(); ++it)
        {
          it->collective_thrust_ += res_thrust;
        }
      }
    }  
  }

  const int log_map_verbose_level = 10;
  if ((FLAGS_v >= log_map_verbose_level) && add_dynamics_factor)
  {
    VLOG(log_map_verbose_level) << "Dynamic measurements:";
    for (auto it=dynamics_measurements.begin(); it!=dynamics_measurements.end(); ++it)
    {
      std::string s;
      const double t = it->timestamp_;
      const Eigen::Vector3d thr = it->collective_thrust_;
      Eigen::Vector3d w;
      if (dynamics_handler_->options_.use_rotational_dynamics)
      {
        w = it->body_rates_;
      }
      else
      {
        w = it->gyro_;
      }
      s = "\nts= " + std::to_string(t) +
      "\nthrust= [" + std::to_string(thr.x()) + ", " + std::to_string(thr.y()) + ", " + std::to_string(thr.z()) +
      "]\nomega= [" + std::to_string(w.x()) + ", " + std::to_string(w.y()) + ", " + std::to_string(w.z()) + "]\n";
      VLOG(log_map_verbose_level) << s;
    }
  }

  // introduce a state for the frame in the backend --------------------------
  if (!backend_.addStates(frame_bundle, imu_measurements, dynamics_measurements, 
                          current_frame_bundle_stamp))
  {
    LOG(ERROR) << "Failed to add state. Will drop frames.";
    return false;
  }

  VLOG(10) << "Backend: Added " << imu_measurements.size() << " inertial measurements.";
  VLOG(10) << "Backend: Added " << dynamics_measurements.size() << " dynamic measurements.";

  return true;
}

void CeresBackendInterface::updateFrameStateWithBackend(
    const FramePtr& f, const bool get_speed_bias)
{
  Transformation T_WS;
  bool success = backend_.get_T_WS(f->bundleId(), T_WS);
  T_WS.getRotation().normalize();
  DEBUG_CHECK(success) << "Could not get state for frame bundle "
                       << f->bundleId() << " from backend";
  f->set_T_w_imu(T_WS);
  if (get_speed_bias)
  {
    SpeedAndBias speed_bias;
    success = backend_.getSpeedAndBias(f->bundleId(), speed_bias);
    DEBUG_CHECK(success) << "Could not get speed/bias for frame bundle "
                         << f->bundleId() << " from backend";
    f->setIMUState(T_WS.getRotation().rotate(speed_bias.block<3, 1>(0, 0)),
                   speed_bias.block<3, 1>(3, 0), speed_bias.block<3, 1>(6, 0));
  }
}

void CeresBackendInterface::updateBundleStateWithBackend(
    const FrameBundlePtr& frames, const bool get_speed_bias)
{
  Transformation T_WS;
  bool success = backend_.get_T_WS(frames->getBundleId(), T_WS);
  DEBUG_CHECK(success) << "Could not get state for frame bundle "
                       << frames->getBundleId() << " from backend";
  frames->set_T_W_B(T_WS);

  if (get_speed_bias)
  {
    SpeedAndBias speed_bias;
    success = backend_.getSpeedAndBias(frames->getBundleId(), speed_bias);
    DEBUG_CHECK(success) << "Could not get speed/bias for frame bundle "
                         << frames->getBundleId() << " from backend";
    frames->setIMUState(T_WS.getRotation().rotate(speed_bias.block<3, 1>(0, 0)),
                        speed_bias.block<3, 1>(3, 0),
                        speed_bias.block<3, 1>(6, 0));
  }
}
void CeresBackendInterface::reset()
{
  VLOG(1) << "Backend: Reset";
  //! @todo implement!
  LOG(ERROR) << "Resetting ceres backend not implemented";
}

void CeresBackendInterface::startThread()
{
  CHECK(thread_ == nullptr) << "Tried to start thread that is already running!";
  stop_thread_ = false;
  thread_.reset(
      new std::thread(&CeresBackendInterface::optimizationLoop, this));
}

void CeresBackendInterface::quitThread()
{
  VLOG(1) << "Interrupting and stopping optimization thread.";
  stop_thread_ = true;
  if (thread_ != nullptr)
  {
    wait_condition_.notify_all();
    thread_->join();
    thread_.reset();
  }
  VLOG(1) << "Thread stopped and joined.";
}

// Performance monitor for benchmarking
void CeresBackendInterface::setPerformanceMonitor(const std::string& trace_dir)
{
  // Initialize Performance Monitor
  g_permon_backend_.reset(new vk::PerformanceMonitor());
  g_permon_backend_->addLog("tot_time");
  g_permon_backend_->addLog("ceres_time");
  g_permon_backend_->addLog("pre_optim_time");
  g_permon_backend_->addLog("marginalization");
  g_permon_backend_->addLog("fixation");
  g_permon_backend_->addLog("n_fixed_lm");
  for (const auto k : MarginalizationTiming::names_)
  {
    g_permon_backend_->addLog(k);
  }
  g_permon_backend_->init("trace_backend", trace_dir);
}

void CeresBackendInterface::optimizationLoop()
{
  VLOG(1) << "Backend: Optimization thread started.";
  while (!stop_thread_)
  {
    {
      std::unique_lock<std::mutex> lock(mutex_backend_);
      wait_condition_.wait(lock, [&] {
        return ((last_added_nframe_images_ != last_optimized_nframe_.load())) ||
               stop_thread_;
      });
      if (stop_thread_)
      {
        return;
      }

      vk::Timer timer;
      {
        std::lock_guard<std::mutex> lock(w_T_correction_mut_);
        if (is_w_T_valid_)
        {
          backend_.removeAllPoseFixation();
          backend_.transformMap(
              w_T_correction_to_apply_,
              optimizer_options_.remove_marginalization_term_after_correction_,
              optimizer_options_.recalculate_imu_terms_after_loop);
          for (const FramePtr& f : active_keyframes_)
          {
            f->accumulated_w_T_correction_ =
                w_T_correction_to_apply_ * f->accumulated_w_T_correction_;
          }
          backend_.setOldestFrameFixed();
          is_w_T_valid_ = false;
        }
      }

      timer.start();
      MarginalizationTiming mag_timing;
      // Marginalization -------------------------------------------------------
      if (optimizer_options_.marginalize)
      {
        // add 1 here because we perform marginalization before optimization, so
        // we want to keep one more frame than after
        if (!backend_.applyMarginalizationStrategy(
                optimizer_options_.num_keyframes,
                optimizer_options_.num_imu_frames + 1, &mag_timing))
        {
          LOG(ERROR) << "Marginalization failed!";
        }
        updateActiveKeyframes();
      }
      if (g_permon_backend_)
      {
        g_permon_backend_->log("marginalization", timer.stop());
        for (const auto k : MarginalizationTiming::names_)
        {
          g_permon_backend_->log(k, mag_timing.get(k));
        }
      }

      // update fixation
      timer.start();
      if (!backend_.needPoseFixation())
      {
        backend_.removeAllPoseFixation();
        backend_.setAllFixedLandmarksEnabled(true);
        lock_to_fixed_landmarks_ = true;
      }
      else
      {
        backend_.setAllFixedLandmarksEnabled(false);
        if (!backend_.hasFixedPose())
        {
          backend_.setOldestFrameFixed();
        }
        lock_to_fixed_landmarks_ = false;
      }
      if (g_permon_backend_)
      {
        g_permon_backend_->log("fixation", timer.stop());
      }

      // Log Backend Map -------------------------------------------------------
      const int log_map_verbose_level = 1000;
      if (FLAGS_v >= log_map_verbose_level)
      {
        // Only partial logging (no landmarks) is supported at the moment.
        VLOG(log_map_verbose_level) << "--- Backend: Inertial Map ---";
        std::shared_ptr<ceres_backend::Map> curr_map;
        curr_map = backend_.getMap();

        // Parameters (no vision)
        std::unordered_map<uint64_t,
              std::shared_ptr<ceres_backend::ParameterBlock> > idToParameterBlockMap;
        idToParameterBlockMap = curr_map->idToParameterBlockMap();

        size_t nPoseParamBlocks = 0;
        size_t nSpeedAndBiasParamBlocks = 0;
        size_t nExternalForceParamBlocks = 0;
        for(auto& p: idToParameterBlockMap)
        {
          if(p.second->typeInfo()=="PoseParameterBlock")    
          {
            nPoseParamBlocks++;
          }
          else if (p.second->typeInfo()=="SpeedAndBiasParameterBlock")
          {
            nSpeedAndBiasParamBlocks++;
          }
          else if (p.second->typeInfo()=="ExternalForceParameterBlock")
          {
            nExternalForceParamBlocks++;
          }
        }
        VLOG(log_map_verbose_level) << "Map containes " << nPoseParamBlocks << " Pose parameter blocks.";
        VLOG(log_map_verbose_level) << "Map containes " << nSpeedAndBiasParamBlocks << " SpeedAndBias parameter blocks.";
        VLOG(log_map_verbose_level) << "Map containes " << nExternalForceParamBlocks << " ExternalForce parameter blocks.";

        // Residuals
        std::unordered_map<ceres::ResidualBlockId, ceres_backend::Map::ResidualBlockSpec>
              residualBlockIdToResidualBlockSpecMap;
        residualBlockIdToResidualBlockSpecMap = curr_map->residualBlockIdToResidualBlockSpecMap();

        size_t nHomogeneousPointError = 0;
        size_t nReprojectionError = 0;
        size_t nSpeedAndBiasError = 0;
        size_t nMarginalizationError = 0;
        size_t nPoseError = 0;
        size_t nIMUError = 0;
        size_t nRelativePoseError = 0;
        size_t nDynamicsError = 0;

        for(auto& r: residualBlockIdToResidualBlockSpecMap)
        {
          switch(r.second.error_interface_ptr->typeInfo())
          {
            case ceres_backend::ErrorType::kHomogeneousPointError : nHomogeneousPointError++; break;
            case ceres_backend::ErrorType::kReprojectionError : nReprojectionError++; break;
            case ceres_backend::ErrorType::kSpeedAndBiasError : nSpeedAndBiasError++; break;
            case ceres_backend::ErrorType::kMarginalizationError : nMarginalizationError++; break;
            case ceres_backend::ErrorType::kPoseError : nPoseError++; break;
            case ceres_backend::ErrorType::kIMUError : nIMUError++; break;
            case ceres_backend::ErrorType::kRelativePoseError : nRelativePoseError++; break;
            case ceres_backend::ErrorType::kDynamicsError : nDynamicsError++; break;
          }
        }
        size_t nResiduals = nHomogeneousPointError + nReprojectionError + nSpeedAndBiasError +
              nMarginalizationError + nPoseError + nIMUError + nRelativePoseError + nDynamicsError;
        VLOG(log_map_verbose_level) << "Map containes " << nResiduals << " residuals:";
        VLOG(log_map_verbose_level) << " - nHomogeneousPointError " << nHomogeneousPointError;
        VLOG(log_map_verbose_level) << " - nReprojectionError " << nReprojectionError;
        VLOG(log_map_verbose_level) << " - nSpeedAndBiasError " << nSpeedAndBiasError;
        VLOG(log_map_verbose_level) << " - nMarginalizationError " << nMarginalizationError;
        VLOG(log_map_verbose_level) << " - nPoseError " << nPoseError;
        VLOG(log_map_verbose_level) << " - nIMUError " << nIMUError;
        VLOG(log_map_verbose_level) << " - nRelativePoseError " << nRelativePoseError;
        VLOG(log_map_verbose_level) << " - nDynamicsError " << nDynamicsError;
      }
      // End log Ceres Map ----------------------------------------------------------

      // Optimization ----------------------------------------------------------
      if (g_permon_backend_)
      {
        g_permon_backend_->log("n_fixed_lm", backend_.numFixedLandmarks());
      }
      timer.start();
      {
        if (skip_optimization_once_)
        {
          skip_optimization_once_ = false;
        }
        else
        {
          backend_.optimize(optimizer_options_.num_iterations,
                            optimizer_options_.num_threads,
                            optimizer_options_.verbose);
        }
      }
      if (g_permon_backend_)
      {
        g_permon_backend_->log("ceres_time", timer.stop());
      }

      last_optimized_nframe_.store(last_added_nframe_images_);
      if (g_permon_backend_)
      {
        g_permon_backend_->log("tot_time",
                               timers_[last_optimized_nframe_.load()].stop());
        g_permon_backend_->writeToFile();
        timers_.erase(timers_.find(last_optimized_nframe_.load()));
      }

      // Publish pose and visualize makers
      Transformation T_WS;
      bool success = backend_.get_T_WS(last_optimized_nframe_.load(), T_WS);
      double last_state_timestamp = backend_.timestamp(last_optimized_nframe_.load());
      DEBUG_CHECK(success) << "Could not get latest Transformation from ceres "
                              "optimizer";
      SpeedAndBias speed_and_bias;
      success =
          backend_.getSpeedAndBias(last_optimized_nframe_, speed_and_bias);
      DEBUG_CHECK(success) << "Could not get latest speed/bias from ceres "
                              "optimizer";
      last_state_.set_T_W_B(T_WS);
      last_state_.set_W_v_B(speed_and_bias.head<3>());
      last_state_.setAccBias(speed_and_bias.tail<3>());
      last_state_.setGyroBias(speed_and_bias.segment<3>(3));
      last_state_timestamp_ns_ = last_state_timestamp*1e9;

      // logging external force
      bool has_external_force = false;
      ExternalForce external_force;
      if (dynamics_handler_->options_.use_dynamics)
      {
        // external force is added to the last_optimized_nframe_ - 1
        BundleId last_optimized_nframe_min_1 = last_optimized_nframe_.load()-1;
        if (last_optimized_nframe_min_1 >= 0)
        {
          has_external_force = backend_.getExternalForce(last_optimized_nframe_min_1, external_force);
        }        
        if (!has_external_force)
        {
          VLOG(log_map_verbose_level) << "Could not get the latest external force from ceres optimizer";
        }
      }

      // logging dynamics measurements
      DynamicsMeasurements dynamics_measurements_to_pub;
      if ((dynamics_handler_->options_.use_dynamics) && (dynamics_handler_->options_.publish_dynamics))
      {
        backend_.getDynamicsMeasurements(dynamics_measurements_to_pub);
      }

      // debug (This is used in combination with the one in addStates... 
      // to publish dynamics but to not use them in the backend)
      // external_force = ExternalForce::Zero();
      // has_external_force = true;
      // end

      // publish current estimation
      if (publisher_)
      {
        if (has_external_force)
        {
          // publisher_->publishWithDynamics(
          //   last_state_, last_added_frame_stamp_ns_, 
          //   external_force, before_the_last_added_frame_stamp_ns_, 
          //   dynamics_measurements_to_pub,
          //   dynamics_handler_->options_.use_rotational_dynamics, 
          //   last_optimized_nframe_.load());

          publisher_->publishWithDynamics(
            last_state_, last_state_timestamp_ns_, 
            external_force, before_the_last_added_frame_stamp_ns_, 
            dynamics_measurements_to_pub,
            dynamics_handler_->options_.use_rotational_dynamics, 
            last_optimized_nframe_.load());
        }
        else
        {
          // publisher_->publish(
          //   last_state_, last_added_frame_stamp_ns_, last_optimized_nframe_.load());

          publisher_->publish(
            last_state_, last_state_timestamp_ns_, last_optimized_nframe_.load());
        }
      }
    }  // release backend mutex.
  }

  VLOG(1) << "Optimization thread ended.";
}

// Set the IMU and the parameters in backend
void CeresBackendInterface::setImu(
    const std::shared_ptr<ImuHandler> imu_handler)
{
  imu_handler_ = imu_handler;

  ImuParameters imu_parameters;
  imu_parameters.a_max = imu_handler_->imu_calib_.saturation_accel_max;
  imu_parameters.g_max = imu_handler_->imu_calib_.saturation_omega_max;
  imu_parameters.sigma_g_c = imu_handler_->imu_calib_.gyro_noise_density;
  imu_parameters.sigma_bg = imu_handler_->imu_init_.omega_bias_sigma;
  imu_parameters.sigma_a_c = imu_handler_->imu_calib_.acc_noise_density;
  imu_parameters.sigma_ba = imu_handler_->imu_init_.acc_bias_sigma;
  imu_parameters.sigma_gw_c =
      imu_handler_->imu_calib_.gyro_bias_random_walk_sigma;
  imu_parameters.sigma_aw_c =
      imu_handler_->imu_calib_.acc_bias_random_walk_sigma;
  imu_parameters.g = imu_handler_->imu_calib_.gravity_magnitude;
  imu_parameters.a0 = imu_handler_->getAccelerometerBias();
  imu_parameters.rate = imu_handler_->imu_calib_.imu_rate;
  imu_parameters.delay_imu_cam = imu_handler_->imu_calib_.delay_imu_cam;
  backend_.addImu(imu_parameters);
}

// Set the dynamics handler backend
void CeresBackendInterface::setDynamicsHandler(const std::shared_ptr<DynamicsHandler> dynamics_handler)
{
  dynamics_handler_ = dynamics_handler;
  backend_.addDynamics(dynamics_handler->options_);
}

// Set the dynamics net
void CeresBackendInterface::setThrustDynamicsNet(const std::shared_ptr<DynamicsNet> dynamics_net)
{
  thrust_dynamics_net_ = dynamics_net;
}

// Set the dynamics net
void CeresBackendInterface::setTorqueDynamicsNet(const std::shared_ptr<DynamicsNet> dynamics_net)
{
  torque_dynamics_net_ = dynamics_net;
}

void CeresBackendInterface::setOptimizeQuadAngVelSpline(
  const std::shared_ptr<OptimizeQuadAngVelSpline> opt_quad_angvel_spline)
{
  opt_quad_angvel_spline_ = opt_quad_angvel_spline;
}

void CeresBackendInterface::setGroundtruthPoses(PoseMeasurements& gt_poses)
{
  backend_.loadGroundtruthPoses(gt_poses);
}

// Start timer for benchmarking
void CeresBackendInterface::startTimer(const BundleId bundle_id)
{
  timers_[bundle_id].start();
}

void CeresBackendInterface::setCorrectionInWorld(
    const Transformation& w_T_correction)
{
  std::lock_guard<std::mutex> lock(w_T_correction_mut_);
  w_T_correction_to_apply_ = w_T_correction;
  is_w_T_valid_ = true;
}

void CeresBackendInterface::getAllActiveKeyframes(
    std::vector<FramePtr>* keyframes)
{
  CHECK_NOTNULL(keyframes);
  keyframes->clear();
  keyframes->insert(keyframes->begin(), active_keyframes_.begin(),
                    active_keyframes_.end());
}

// Keep track of active keyframes (single frames that were frontend keyframes
// and that are still in backend)
void CeresBackendInterface::updateActiveKeyframes()
{
  // Find the oldest keyframe in backend
  BundleId oldest_keyframe_in_backend = backend_.oldestKeyframeBundleId();
  if (oldest_keyframe_in_backend == -1)
  {
    active_keyframes_.clear();
    return;
  }
  // NFrames are only marginalized sequentially, delete until ID matches
  while (!active_keyframes_.empty())
  {
    if (oldest_keyframe_in_backend == active_keyframes_.front()->bundleId())
    {
      return;
    }
    VLOG(40) << "Backend: marginalized frame with id "
             << active_keyframes_.front()->id();
    active_keyframes_.pop_front();
  }
}

std::string CeresBackendInterface::getStationaryStatusStr() const
{
  std::string str("Static: ");
  if (image_motion_detector_stationary_)
  {
    str += "Visual ";
  }
  if (imu_motion_detector_stationary_)
  {
    str += "Inertial ";
  }

  if (!image_motion_detector_stationary_ && !imu_motion_detector_stationary_)
  {
    str = "Moving";
  }
  return str;
}

void CeresBackendInterface::getLatestSpeedBiasPose(
    Eigen::Matrix<double, 9, 1>* speed_bias, Transformation* T_WS,
    double* timestamp) const
{
  backend_.get_T_WS(backend_.currentBundleId(), *T_WS);
  backend_.getSpeedAndBias(backend_.currentBundleId(), *speed_bias);
  *timestamp = backend_.timestamp(backend_.currentFrameId());
}

void CeresBackendInterface::setReinitStartValues(
    const Eigen::Matrix<double, 9, 1>& sb, const Transformation& Tws,
    const double timestamp)
{
  backend_.is_reinit_ = true;
  backend_.reinit_speed_bias_ = sb;
  backend_.reinit_T_WS_ = Tws;
  backend_.reinit_timestamp_start_ = timestamp;
}


}  // namespace svo
