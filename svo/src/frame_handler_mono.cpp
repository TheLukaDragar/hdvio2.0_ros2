// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// This file is subject to the terms and conditions defined in the file
// 'LICENSE', which is part of this source code package.

#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/common/frame.h>
#include <svo/common/point.h>
#include <svo/common/logging.h>
#include <svo/img_align/sparse_img_align.h>
#include <svo/abstract_bundle_adjustment.h>
#include <svo/direct/depth_filter.h>
#include <svo/tracker/feature_tracking_types.h>
#include <svo/initialization.h>
#include <svo/direct/feature_detection.h>
#include <svo/direct/feature_detection_utils.h>
#include <svo/reprojector.h>
#include <vikit/performance_monitor.h>

namespace svo {

FrameHandlerMono::FrameHandlerMono(
    const BaseOptions& base_options,
    const DepthFilterOptions& depth_filter_options,
    const DetectorOptions& feature_detector_options,
    const InitializationOptions& init_options,
    const ReprojectorOptions& reprojector_options,
    const FeatureTrackerOptions& tracker_options,
    const CameraBundle::Ptr& cam)
  : FrameHandlerBase(
      base_options, reprojector_options, depth_filter_options,
      feature_detector_options, init_options, tracker_options, cam)
{ ; }

UpdateResult FrameHandlerMono::processFrameBundle()
{
  LOG(INFO) << "[FRAME] Processing frame bundle in stage: " << (int)stage_;
  UpdateResult res = UpdateResult::kFailure;
  if(stage_ == Stage::kTracking)
  {
    LOG(INFO) << "[FRAME] Processing in TRACKING mode";
    res = processFrame();
  }
  else if(stage_ == Stage::kInitializing)
  {
    LOG(INFO) << "[FRAME] Processing in INITIALIZING mode";
    res = processFirstFrame();
  }
  else if(stage_ == Stage::kRelocalization)
  {
    LOG(INFO) << "[FRAME] Processing in RELOCALIZATION mode";
    res = relocalizeFrame(Transformation(), reloc_keyframe_);
  }
  else if (stage_ == Stage::kReinitialize)
  {
    LOG(INFO) << "[FRAME] Processing in REINITIALIZE mode";
    res = reinitialize(reloc_keyframe_);
  }
  
  LOG(INFO) << "[FRAME] Processing result: " << (int)res;
  return res;
}

void FrameHandlerMono::addImage(
    const cv::Mat& img,
    const uint64_t timestamp)
{
  addImageBundle({img}, timestamp);
}

UpdateResult FrameHandlerMono::processFirstFrame()
{
  if(!initializer_->have_depth_prior_)
  {
    initializer_->setDepthPrior(options_.init_map_scale);
  }
  if(have_rotation_prior_)
  {
    VLOG(2) << "Setting absolute orientation prior";
    initializer_->setAbsoluteOrientationPrior(
          newFrame()->T_cam_imu().getRotation() * R_imu_world_);
  }
  const auto res = initializer_->addFrameBundle(new_frames_);

  if(res == InitResult::kTracking)
    return UpdateResult::kDefault;

  // make old frame keyframe
  initializer_->frames_ref_->setKeyframe();
  initializer_->frames_ref_->at(0)->setKeyframe();
  if(bundle_adjustment_type_==BundleAdjustmentType::kCeres)
  {
     map_->addKeyframe(initializer_->frames_ref_->at(0),
                       bundle_adjustment_type_==BundleAdjustmentType::kCeres);
  }
  else if(bundle_adjustment_type_==BundleAdjustmentType::kGtsam)
  {
    CHECK(bundle_adjustment_)
        << "bundle_adjustment_type_ is kGtsam but bundle_adjustment_ is NULL";
    bundle_adjustment_->bundleAdjustment(initializer_->frames_ref_);
  }
  else
  {
    map_->addKeyframe(initializer_->frames_ref_->at(0), false);
  }
  // make new frame keyframe
  newFrame()->setKeyframe();
  frame_utils::getSceneDepth(newFrame(), depth_median_, depth_min_, depth_max_);
  VLOG(40) << "Current Frame Depth: " << "min: " << depth_min_
          << ", max: " << depth_max_ << ", median: " << depth_median_;
  depth_filter_->addKeyframe(
              newFrame(), depth_median_, 0.5*depth_min_, depth_median_*1.5);
  VLOG(40) << "Updating seeds in second frame using last frame...";
  depth_filter_->updateSeeds({ newFrame() }, lastFrameUnsafe());

  // add frame to map

  map_->addKeyframe(newFrame(),
                    bundle_adjustment_type_==BundleAdjustmentType::kCeres);
  LOG(INFO) << "SVO Stage Transition: kInitializing -> kTracking (initialization complete)";
  stage_ = Stage::kTracking;
  tracking_quality_ = TrackingQuality::kGood;
  initializer_->reset();
  VLOG(1) << "Init: Selected second frame, triangulated initial map.";
  return UpdateResult::kKeyframe;
}

UpdateResult FrameHandlerMono::processFrame()
{
  static int frame_count = 0;
  frame_count++;
  LOG(INFO) << "[TRACKING] ===== Processing frame #" << frame_count << " in tracking mode =====";
  VLOG(40) << "Updating seeds in overlapping keyframes...";
  // this is useful when the pipeline is with the backend,
  // where we should have more accurate pose at this moment
  depth_filter_->updateSeeds(overlap_kfs_.at(0), lastFrame());

  // ---------------------------------------------------------------------------
  // tracking

  // STEP 1: Sparse Image Align
  VLOG(40) << "===== Sparse Image Alignment =====";
  size_t n_total_observations = 0;
  sparseImageAlignment();

  // STEP 2: Map Reprojection & Feature Align
  VLOG(40) << "===== Project Map to Current Frame =====";
  LOG(INFO) << "[TRACKING] Step 2: Map Reprojection & Feature Alignment";
  n_total_observations = projectMapInFrame();
  LOG(INFO) << "[TRACKING] Reprojection found " << n_total_observations << " features (min required: " << options_.quality_min_fts << ")";
  if(n_total_observations < options_.quality_min_fts)
  {
    LOG(ERROR) << "[TRACKING] FAILURE: Not enough features after reprojection: "
                 << n_total_observations << " < " << options_.quality_min_fts;
    return UpdateResult::kFailure;
  }

  // STEP 3: Pose & Structure Optimization
  // redundant when using ceres backend
  LOG(INFO) << "[TRACKING] Step 3: Pose optimization (backend type: " << (bundle_adjustment_type_==BundleAdjustmentType::kCeres ? "Ceres" : "SVO") << ")";
  if(bundle_adjustment_type_!=BundleAdjustmentType::kCeres)
  {
    VLOG(40) << "===== Pose Optimization =====";
    LOG(INFO) << "[TRACKING] Step 3: Pose Optimization";
    n_total_observations = optimizePose();
    LOG(INFO) << "[TRACKING] Step 3: Pose optimization completed with " << n_total_observations << " features";
    if(n_total_observations < options_.quality_min_fts)
    {
      LOG(ERROR) << "[TRACKING] FAILURE: Not enough features after pose optimization: "
                   << n_total_observations << " < " << options_.quality_min_fts;
      return UpdateResult::kFailure;
    }
    LOG(INFO) << "[TRACKING] Step 3b: Starting structure optimization";
    optimizeStructure(new_frames_, options_.structure_optimization_max_pts, 5);
    LOG(INFO) << "[TRACKING] Step 3b: Structure optimization completed";
  }

  // return if tracking bad
  setTrackingQuality(n_total_observations);
  LOG(INFO) << "[TRACKING] Quality check: " << n_total_observations << " observations, quality = " << (int)tracking_quality_;
  if(tracking_quality_ == TrackingQuality::kInsufficient)
  {
    LOG(ERROR) << "[TRACKING] FAILURE: Tracking quality insufficient";
    return UpdateResult::kFailure;
  }

  // ---------------------------------------------------------------------------
  // select keyframe
  VLOG(40) << "===== Keyframe Selection =====";
  frame_utils::getSceneDepth(newFrame(), depth_median_, depth_min_, depth_max_);
  VLOG(40) << "Current Frame Depth: " << "min: " << depth_min_
           << ", max: " << depth_max_ << ", median: " << depth_median_;
  initializer_->setDepthPrior(depth_median_);
  initializer_->have_depth_prior_ = true;
  bool need_kf = need_new_kf_(newFrame()->T_f_w_);
  LOG(INFO) << "[TRACKING] Keyframe selection: need_new_kf=" << need_kf << ", quality=" << (int)tracking_quality_ << ", stage=" << (int)stage_;
  if(!need_kf
     || tracking_quality_ == TrackingQuality::kBad
     || stage_ == Stage::kRelocalization)
  {
    if(tracking_quality_ == TrackingQuality::kGood)
    {
      LOG(INFO) << "[TRACKING] ✓ Frame processed successfully - CONTINUOUS TRACKING MODE (no keyframe needed)";
      LOG(INFO) << "[TRACKING] → Tracking " << n_total_observations << " features, quality=Good, system running normally";
      VLOG(40) << "Updating seeds in overlapping keyframes...";
      CHECK(!overlap_kfs_.empty());
      // now the seed is updated at the beginning of next frame
//      depth_filter_->updateSeeds(overlap_kfs_.at(0), newFrame());
    }
    return UpdateResult::kDefault;
  }
  newFrame()->setKeyframe();
  LOG(INFO) << "[TRACKING] NEW KEYFRAME SELECTED! Adding to map.";
  VLOG(40) << "New keyframe selected.";

  // ---------------------------------------------------------------------------
  // new keyframe selected
  if (depth_filter_->options_.extra_map_points)
  {
    depth_filter_->sec_feature_detector_->resetGrid();
    OccupandyGrid2D map_point_grid(depth_filter_->sec_feature_detector_->grid_);
    reprojector_utils::reprojectMapPoints(
          newFrame(), overlap_kfs_.at(0),
          reprojectors_.at(0)->options_, &map_point_grid);
    DepthFilter::ulock_t lock(depth_filter_->feature_detector_mut_);
    feature_detection_utils::mergeGrids(
          map_point_grid, &depth_filter_->sec_feature_detector_->grid_);
  }
  upgradeSeedsToFeatures(newFrame());
  // init new depth-filters, set feature-detection grid-cells occupied that
  // already have a feature
  //
  // TODO: we should also project all seeds first! to make sure that we don't
  //       initialize seeds in the same location!!! this can be done in the
  //       depth-filter
  //
  {
    DepthFilter::ulock_t lock(depth_filter_->feature_detector_mut_);
    setDetectorOccupiedCells(0, depth_filter_->feature_detector_);

  } // release lock
  depth_filter_->addKeyframe(
        newFrame(), depth_median_, 0.5*depth_min_, depth_median_*1.5);

  if(options_.update_seeds_with_old_keyframes)
  {
    VLOG(40) << "Updating seeds in current frame using last frame...";
    depth_filter_->updateSeeds({ newFrame() }, lastFrameUnsafe());
    VLOG(40) << "Updating seeds in current frame using overlapping keyframes...";
    for(const FramePtr& old_keyframe : overlap_kfs_.at(0))
      depth_filter_->updateSeeds({ newFrame() }, old_keyframe);
  }

  // VLOG(40) << "Updating seeds in overlapping keyframes...";
//  depth_filter_->updateSeeds(overlap_kfs_.at(0), newFrame());

  // add keyframe to map
  map_->addKeyframe(newFrame(),
                    bundle_adjustment_type_==BundleAdjustmentType::kCeres);

  // if limited number of keyframes, remove the one furthest apart
  if(options_.max_n_kfs > 2)
  {
    while(map_->size() > options_.max_n_kfs)
    {
      if(bundle_adjustment_type_==BundleAdjustmentType::kCeres)
      {
        // deal differently with map for ceres backend
        map_->removeOldestKeyframe();
      }
      else
      {
        FramePtr furthest_frame = map_->getFurthestKeyframe(newFrame()->pos());
        map_->removeKeyframe(furthest_frame->id());
      }
    }
  }
  return UpdateResult::kKeyframe;
}

UpdateResult FrameHandlerMono::relocalizeFrame(
    const Transformation& /*T_cur_ref*/,
    const FramePtr& ref_keyframe)
{
  ++relocalization_n_trials_;
  if(ref_keyframe == nullptr)
    return UpdateResult::kFailure;

  VLOG_EVERY_N(1, 20) << "Relocalizing frame";
  FrameBundle::Ptr ref_frame(new FrameBundle({ref_keyframe}, ref_keyframe->bundleId()));
  last_frames_ = ref_frame;
  UpdateResult res = processFrame();
  if(res == UpdateResult::kDefault)
  {
    // Reset to default mode.
    LOG(INFO) << "SVO Stage Transition: kRelocalization -> kTracking (relocalization successful)";
    stage_ = Stage::kTracking;
    relocalization_n_trials_ = 0;

    VLOG(1) << "Relocalization successful.";
    SVO_INFO_STREAM("Relocalization successful.");
  }
  else
  {
    // reset to last well localized pose
    newFrame()->T_f_w_ = ref_keyframe->T_f_w_;
  }
  return res;
}

UpdateResult FrameHandlerMono::reinitialize(const FramePtr& ref_keyframe)
{
  VLOG(1) << "Reinitializing.";
  SVO_INFO_STREAM("Reinitializing.");

  relocalization_n_trials_ = 0; 
  
  if(ref_keyframe == nullptr)
    return UpdateResult::kFailure;

  FrameBundle::Ptr ref_frame(new FrameBundle({ref_keyframe}, ref_keyframe->bundleId()));
  last_frames_ = ref_frame;

  // ---------------------------------------------------------------------------
  // tracking
  // try again to track from the most recent keyframe

  // STEP 1: Sparse Image Align
  VLOG(40) << "===== Sparse Image Alignment =====";
  size_t n_total_observations = 0;
  sparseImageAlignment();

  // STEP 2: Map Reprojection & Feature Align
  VLOG(40) << "===== Project Map to Current Frame =====";
  n_total_observations = projectMapInFrame();
  // If sparse and feature alignments are successful, we do not need to reinitialize
  if(n_total_observations > options_.quality_min_fts)
  {
    VLOG(1) << "Alignment successful. Do not initialize !";
    SVO_INFO_STREAM("Alignment successful. Do not initialize.");

    LOG(INFO) << "SVO Stage Transition: kReinitialize -> kTracking (alignment successful)";
    stage_ = Stage::kTracking; 
    return UpdateResult::kDefault;
  }

  // ---------------------------------------------------------------------------
  // select keyframe
  VLOG(40) << "===== Keyframe Selection =====";
  frame_utils::getSceneDepth(newFrame(), depth_median_, depth_min_, depth_max_);
  // @todo :  needed ?
  // VLOG(40) << "Current Frame Depth: " << "min: " << depth_min_
  //          << ", max: " << depth_max_ << ", median: " << depth_median_;
  // initializer_->setDepthPrior(depth_median_);
  // initializer_->have_depth_prior_ = true;

  newFrame()->setKeyframe();
  
  VLOG(1) << "New keyframe selected during reinitialization.";
  SVO_INFO_STREAM("Adding a Keyframe during reinitialization!");

  upgradeSeedsToFeatures(newFrame());
  // init new depth-filters, set feature-detection grid-cells occupied that
  // already have a feature
  {
    DepthFilter::ulock_t lock(depth_filter_->feature_detector_mut_);
    setDetectorOccupiedCells(0, depth_filter_->feature_detector_);

  } // release lock
  depth_filter_->addKeyframe(
        newFrame(), depth_median_, 0.5*depth_min_, depth_median_*1.5);

  // add keyframe to map
  map_->addKeyframe(newFrame(),
                    bundle_adjustment_type_==BundleAdjustmentType::kCeres);

  // if limited number of keyframes, remove the one furthest apart
  if(options_.max_n_kfs > 2)
  {
    while(map_->size() > options_.max_n_kfs)
    {
      if(bundle_adjustment_type_==BundleAdjustmentType::kCeres)
      {
        // deal differently with map for ceres backend
        map_->removeOldestKeyframe();
      }
      else
      {
        FramePtr furthest_frame = map_->getFurthestKeyframe(newFrame()->pos());
        map_->removeKeyframe(furthest_frame->id());
      }
    }
  }

  stage_ = Stage::kTracking;
  return UpdateResult::kKeyframe;
}

void FrameHandlerMono::resetAll()
{
  if(bundle_adjustment_type_==BundleAdjustmentType::kCeres)
  {
    // with the ceres backend we have to make sure to initialize the scale
    backend_scale_initialized_ = false;
  }
  else
  {
    backend_scale_initialized_ = true;
  }
  resetVisionFrontendCommon();
}

FramePtr FrameHandlerMono::lastFrame() const
{
  return (last_frames_ == nullptr) ? nullptr : last_frames_->at(0);
}

const FramePtr& FrameHandlerMono::newFrame() const
{
  return new_frames_->frames_[0];
}

const FramePtr& FrameHandlerMono::lastFrameUnsafe() const
{
  return last_frames_->frames_[0];
}

bool FrameHandlerMono::haveLastFrame() const
{
  return (last_frames_ != nullptr);
}

} // namespace svo
