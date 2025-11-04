#include <ament_index_cpp/get_package_share_directory.hpp>

#include <svo/svo.h>
#include <svo/common/imu_calibration.h>
#include <hdvio2/svo_factory.h>
#include <svo/frame_handler_mono.h>
#include <svo/frame_handler_stereo.h>
#include <vikit/params_helper.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <yaml-cpp/yaml.h>
#pragma diagnostic pop

namespace svo {
namespace factory {

BaseOptions loadBaseOptions(const rclcpp::Node::SharedPtr& pnh, bool forward_default)
{
  BaseOptions o;
  o.max_n_kfs = pnh->declare_parameter("max_n_kfs", 5);
  o.use_imu = pnh->declare_parameter("use_imu", false);
  o.trace_dir = pnh->declare_parameter<std::string>("trace_dir", ament_index_cpp::get_package_share_directory("svo")+"/trace");
  o.quality_min_fts = vk::param<int>(pnh, "quality_min_fts", 50);
  o.quality_max_fts_drop = vk::param<int>(pnh, "quality_max_drop_fts", 40);
  o.relocalization_max_trials = vk::param<int>(pnh, "relocalization_max_trials", 50);
  o.poseoptim_prior_lambda = vk::param<double>(pnh, "poseoptim_prior_lambda", 0.0);
  o.poseoptim_using_unit_sphere = vk::param<bool>(pnh, "poseoptim_using_unit_sphere", false);
  o.img_align_prior_lambda_rot = vk::param<double>(pnh, "img_align_prior_lambda_rot", 0.0);
  o.img_align_prior_lambda_trans = vk::param<double>(pnh, "img_align_prior_lambda_trans", 0.0);
  o.structure_optimization_max_pts = vk::param<int>(pnh, "structure_optimization_max_pts", 20);
  o.init_map_scale = vk::param<double>(pnh, "map_scale", 1.0);
  std::string default_kf_criterion = forward_default ? "FORWARD" : "DOWNLOOKING";
  if(vk::param<std::string>(pnh, "kfselect_criterion", default_kf_criterion) == "FORWARD")
    o.kfselect_criterion = KeyframeCriterion::FORWARD;
  else
    o.kfselect_criterion = KeyframeCriterion::DOWNLOOKING;
  o.kfselect_min_dist = vk::param<double>(pnh, "kfselect_min_dist", 0.12);
  o.kfselect_numkfs_upper_thresh = vk::param<int>(pnh, "kfselect_numkfs_upper_thresh", 120);
  o.kfselect_numkfs_lower_thresh = vk::param<double>(pnh, "kfselect_numkfs_lower_thresh", 70);
  o.kfselect_min_dist_metric = vk::param<double>(pnh, "kfselect_min_dist_metric", 0.01);
  o.kfselect_min_angle = vk::param<double>(pnh, "kfselect_min_angle", 20);
  o.kfselect_min_disparity = vk::param<double>(pnh, "kfselect_min_disparity", 40);
  o.kfselect_min_num_frames_between_kfs = vk::param<int>(pnh, "kfselect_min_num_frames_between_kfs", 2);
  o.kfselect_backend_max_time_sec = vk::param<double>(pnh, "kfselect_backend_max_time_sec", 3.0);
  o.img_align_max_level = vk::param<int>(pnh, "img_align_max_level", 4);
  o.img_align_min_level = vk::param<int>(pnh, "img_align_min_level", 2);
  o.img_align_robustification = vk::param<bool>(pnh, "img_align_robustification", false);
  o.img_align_use_distortion_jacobian =
      vk::param<bool>(pnh, "img_align_use_distortion_jacobian", false);
  o.img_align_est_illumination_gain =
      vk::param<bool>(pnh, "img_align_est_illumination_gain", false);
  o.img_align_est_illumination_offset =
      vk::param<bool>(pnh, "img_align_est_illumination_offset", false);
  o.poseoptim_thresh = vk::param<double>(pnh, "poseoptim_thresh", 2.0);
  o.update_seeds_with_old_keyframes =
      vk::param<bool>(pnh, "update_seeds_with_old_keyframes", true);
  o.use_async_reprojectors = vk::param<bool>(pnh, "use_async_reprojectors", false);
  o.trace_statistics = vk::param<bool>(pnh, "trace_statistics", false);
  o.backend_scale_stable_thresh =
      vk::param<double>(pnh, "backend_scale_stable_thresh", 0.02);
  o.global_map_lc_timeout_sec_ =
      vk::param<double>(pnh, "global_map_timeout_sec", 2.0);
  return o;
}

DetectorOptions loadDetectorOptions(const rclcpp::Node::SharedPtr& pnh)
{
  DetectorOptions o;
  o.cell_size = vk::param<int>(pnh, "grid_size", 35);
  o.max_level = vk::param<int>(pnh, "n_pyr_levels", 3) - 1;
  o.threshold_primary = vk::param<int>(pnh, "detector_threshold_primary", 10);
  o.threshold_secondary = vk::param<int>(pnh, "detector_threshold_secondary", 200);
  o.threshold_shitomasi = vk::param<int>(pnh, "detector_threshold_shitomasi", 100);
  if(vk::param<bool>(pnh, "use_edgelets", true))
    o.detector_type = DetectorType::kFastGrad;
  else
    o.detector_type = DetectorType::kFast;
  return o;
}

DepthFilterOptions loadDepthFilterOptions(const rclcpp::Node::SharedPtr& pnh)
{
  DepthFilterOptions o;
  o.max_search_level = vk::param<int>(pnh, "n_pyr_levels", 3) - 1;
  o.use_threaded_depthfilter =
      vk::param<bool>(pnh, "use_threaded_depthfilter", true);
  o.seed_convergence_sigma2_thresh =
      vk::param<double>(pnh, "seed_convergence_sigma2_thresh", 200.0);
  o.mappoint_convergence_sigma2_thresh =
      vk::param<double>(pnh, "mappoint_convergence_sigma2_thresh", 500.0);
  o.scan_epi_unit_sphere = vk::param<bool>(pnh, "scan_epi_unit_sphere", false);
  o.affine_est_offset= vk::param<bool>(pnh, "depth_filter_affine_est_offset", true);
  o.affine_est_gain = vk::param<bool>(pnh, "depth_filter_affine_est_gain", false);
  o.max_n_seeds_per_frame = static_cast<size_t>(
        static_cast<double>(vk::param<int>(pnh, "max_fts", 120))
        * vk::param<double>(pnh, "max_seeds_ratio", 3.0));
  o.max_map_seeds_per_frame = static_cast<size_t>(
        static_cast<double>(vk::param<int>(pnh, "max_map_fts", 120)));
  o.extra_map_points =
      vk::param<bool>(pnh, "depth_filter_extra_map_points", false);
  return o;
}

InitializationOptions loadInitializationOptions(const rclcpp::Node::SharedPtr& pnh)
{
  InitializationOptions o;
  o.init_min_features = vk::param<int>(pnh, "init_min_features", 100);
  o.init_min_tracked = vk::param<int>(pnh, "init_min_tracked", 80);
  o.init_min_inliers = vk::param<int>(pnh, "init_min_inliers", 70);
  o.init_min_disparity = vk::param<double>(pnh, "init_min_disparity", 40.0);
  o.init_min_features_factor = vk::param<double>(pnh, "init_min_features_factor", 2.0);
  o.reproj_error_thresh = vk::param<double>(pnh, "reproj_err_thresh", 2.0);
  o.init_disparity_pivot_ratio = vk::param<double>(pnh, "init_disparity_pivot_ratio", 0.5);
  std::string init_method = vk::param<std::string>(pnh, "init_method", "FivePoint");
  if(init_method == "Homography")
    o.init_type = InitializerType::kHomography;
  else if(init_method == "TwoPoint")
    o.init_type = InitializerType::kTwoPoint;
  else if(init_method == "FivePoint")
    o.init_type = InitializerType::kFivePoint;
  else if(init_method == "OneShot")
    o.init_type = InitializerType::kOneShot;
  else
    SVO_ERROR_STREAM("Initialization Method not supported: " << init_method);
  return o;
}

FeatureTrackerOptions loadTrackerOptions(const rclcpp::Node::SharedPtr& pnh)
{
  FeatureTrackerOptions o;
  o.klt_max_level = vk::param<int>(pnh, "klt_max_level", 4);
  o.klt_min_level = vk::param<int>(pnh, "klt_min_level", 0.001);
  return o;
}

ReprojectorOptions loadReprojectorOptions(const rclcpp::Node::SharedPtr& pnh)
{
  ReprojectorOptions o;
  o.max_n_kfs = vk::param<int>(pnh, "reprojector_max_n_kfs", 5);
  o.max_n_features_per_frame = vk::param<int>(pnh, "max_fts", 160);
  o.cell_size = vk::param<int>(pnh, "grid_size", 35);
  o.reproject_unconverged_seeds =
      vk::param<bool>(pnh, "reproject_unconverged_seeds", true);
  o.max_unconverged_seeds_ratio =
      vk::param<double>(pnh, "max_unconverged_seeds_ratio", -1.0);
  o.min_required_features =
      vk::param<int>(pnh, "quality_min_fts", 50);
  o.seed_sigma2_thresh =
      vk::param<double>(pnh, "seed_convergence_sigma2_thresh", 200.0);

  o.affine_est_offset =
      vk::param<bool>(pnh, "reprojector_affine_est_offset", true);
  o.affine_est_gain =
      vk::param<bool>(pnh, "reprojector_affine_est_gain", false);
  o.max_fixed_landmarks =
      vk::param<int>(pnh, "reprojector_max_fixed_landmarks", 50);
  o.max_n_global_kfs =
      vk::param<int>(pnh, "reprojector_max_n_global_kfs", 20);
  o.use_kfs_from_global_map =
      vk::param<bool>(pnh, "reprojector_use_kfs_from_global_map", false);
  o.fixed_lm_grid_size =
      vk::param<int>(pnh, "reprojector_fixed_lm_grid_size", 50);

  return o;
}

CameraBundle::Ptr loadCameraFromYaml(const rclcpp::Node::SharedPtr& pnh)
{
  std::string calib_file = vk::param<std::string>(pnh, "calib_file", "~/cam.yaml");
  CameraBundle::Ptr ncam = CameraBundle::loadFromYaml(calib_file);
  std::cout << "loaded " << ncam->numCameras() << " cameras";
  for(const auto& cam : ncam->getCameraVector())
    cam->printParameters(std::cout, "");
  return ncam;
}

StereoTriangulationOptions loadStereoOptions(const rclcpp::Node::SharedPtr& pnh)
{
  StereoTriangulationOptions o;
  o.triangulate_n_features = vk::param<int>(pnh, "max_fts", 120);
  o.max_depth_inv = vk::param<double>(pnh, "max_depth_inv", 1.0/50.0);
  o.min_depth_inv = vk::param<double>(pnh, "min_depth_inv", 1.0/0.5);
  o.mean_depth_inv = vk::param<double>(pnh, "mean_depth_inv", 1.0/2.0);
  return o;
}

ImuHandler::Ptr getImuHandler(const rclcpp::Node::SharedPtr& pnh)
{
  std::string calib_file = vk::param<std::string>(pnh, "calib_file", "");
  ImuCalibration imu_calib = ImuHandler::loadCalibrationFromFile(calib_file);
  imu_calib.print("Loaded IMU Calibration");
  ImuInitialization imu_init = ImuHandler::loadInitializationFromFile(calib_file);
  imu_init.print("Loaded IMU Initialization");
  IMUHandlerOptions options;
  options.temporal_stationary_check =
      vk::param<bool>(pnh, "imu_temporal_stationary_check", false);
  options.temporal_window_length_sec_ =
      vk::param<double>(pnh, "imu_temporal_window_length_sec", 0.5);
  options.stationary_acc_sigma_thresh_ =
      vk::param<double>(pnh, "stationary_acc_sigma_thresh", 0.0);
  options.stationary_gyr_sigma_thresh_ =
      vk::param<double>(pnh, "stationary_gyr_sigma_thresh", 0.0);
  ImuHandler::Ptr imu_handler(new ImuHandler(imu_calib, imu_init, options));
  return imu_handler;
}

DynamicsHandler::Ptr getDynamicsHandler(const rclcpp::Node::SharedPtr& pnh)
{
  DynamicsHandlerOptions options;
  std::string calib_file = vk::param<std::string>(pnh, "calib_file", "");
  YAML::Node data = YAML::LoadFile(calib_file);
  if(data["dynamics_params"].IsDefined())
  {
    options.thrust_net_sampling_freq = data["dynamics_params"]["thrust_net_sampling_freq"].as<double>();
    options.thrust_net_input_window_length = data["dynamics_params"]["thrust_net_input_window_length"].as<double>();
    options.torque_net_sampling_freq = data["dynamics_params"]["torque_net_sampling_freq"].as<double>();
    options.torque_net_input_window_length = data["dynamics_params"]["torque_net_input_window_length"].as<double>();
    options.mass = data["dynamics_params"]["mass"].as<double>();
    Eigen::Vector3d r_fr(
      data["dynamics_params"]["r_fr"].as<std::vector<double>>().data());
    options.r_fr = r_fr;
    Eigen::Vector3d r_br(
      data["dynamics_params"]["r_br"].as<std::vector<double>>().data());
    options.r_br = r_br;
    Eigen::Vector3d r_bl(
      data["dynamics_params"]["r_bl"].as<std::vector<double>>().data());
    options.r_bl = r_bl;
    Eigen::Vector3d r_fl(
      data["dynamics_params"]["r_fl"].as<std::vector<double>>().data());
    options.r_fl = r_fl;
    Eigen::Vector3d inertia_diag(
      data["dynamics_params"]["inertia_diag"].as<std::vector<double>>().data());
    Eigen::Matrix3d Inertia(Eigen::Matrix3d::Identity());
    Inertia(0,0) = inertia_diag[0];
    Inertia(1,1) = inertia_diag[1];
    Inertia(2,2) = inertia_diag[2];
    options.Inertia = Inertia;
    Eigen::Vector4d thrust_coeffs(
      data["dynamics_params"]["thrust_coeffs"].as<std::vector<double>>().data());
    options.thrust_coeffs = thrust_coeffs;
    Eigen::Vector4d torque_coeffs(
      data["dynamics_params"]["torque_coeffs"].as<std::vector<double>>().data());
    options.torque_coeffs = torque_coeffs;
    options.n_init_frames = data["dynamics_params"]["n_init_frames"].as<int>();
    options.sigma_bodyrates_c = data["dynamics_params"]["sigma_bodyrates_c"].as<double>();
    options.sigma_gyro_bias_c = data["dynamics_params"]["sigma_gyro_bias_c"].as<double>();
    options.sigma_accel_c = data["dynamics_params"]["sigma_accel_c"].as<double>();
    options.sigma_accel_bias_c = data["dynamics_params"]["sigma_accel_bias_c"].as<double>();
    options.sigma_thrustz_c = data["dynamics_params"]["sigma_thrustz_c"].as<double>();
    options.sigma_thrustx_c = data["dynamics_params"]["sigma_thrustx_c"].as<double>();
    options.sigma_thrusty_c = data["dynamics_params"]["sigma_thrusty_c"].as<double>();
    Eigen::Vector3d sigma_external_force(
      data["dynamics_params"]["sigma_external_force"].as<std::vector<double>>().data());
    options.sigma_external_force = sigma_external_force;
    options.publish_dynamics = data["dynamics_params"]["publish_dynamics"].as<bool>();
    options.dt_knot_s = data["dynamics_params"]["dt_knot_s"].as<double>();
    options.spline_length_s = data["dynamics_params"]["spline_length_s"].as<double>();
    options.max_iter = data["dynamics_params"]["max_iter"].as<int>();
    options.stop_thresh = data["dynamics_params"]["stop_thresh"].as<double>();
    options.mode = data["dynamics_params"]["mode"].as<int>();
    options.debug_info = data["dynamics_params"]["debug_info"].as<bool>();
  }
  else
  {
    LOG(FATAL) << "Could not load dynamics parameters from file";
  }

  bool use_dynamics = vk::param<bool>(pnh, "use_dynamics", false);
  options.use_dynamics = use_dynamics;
  bool external_force_zero_mean = vk::param<bool>(pnh, "external_force_zero_mean", true);
  options.external_force_zero_mean = external_force_zero_mean;
  bool use_learned_residuals = vk::param<bool>(pnh, "use_learned_residuals", false);
  options.use_learned_residuals = use_learned_residuals;
  bool use_rotational_dynamics = vk::param<bool>(pnh, "use_rotational_dynamics", false);
  options.use_rotational_dynamics = use_rotational_dynamics;
  std::string thrust_net_model_file = vk::param<std::string>(pnh, "thrust_net_model_file", "");
  options.thrust_net_model_file = thrust_net_model_file;
  std::string torque_net_model_file = vk::param<std::string>(pnh, "torque_net_model_file", "");
  options.torque_net_model_file = torque_net_model_file;

  SVO_INFO_STREAM("=======================================");
  if (use_dynamics)
  {
    SVO_INFO_STREAM("Using drone dynamics in the backend!");
    if (external_force_zero_mean)
    {
      SVO_INFO_STREAM("External Force Mean is assumed to be 0 (as in VIMO)!");
    }
    else
    {
      SVO_INFO_STREAM("External Force Mean is obtained from mean(accel_i - thrust_i) (as in VID-Fusion)!");
    }

    if (use_learned_residuals)
    {
      SVO_INFO_STREAM("Using learned residuals!");
      SVO_INFO_STREAM("Loading thrust net from: " << thrust_net_model_file);
      if (use_rotational_dynamics)
      {
        SVO_INFO_STREAM("Loading torque net from: " << torque_net_model_file);
      }
    }
    else
    {
      if (external_force_zero_mean)
      {
        SVO_INFO_STREAM("Running VIMO!");
      }
      else
      {
        SVO_INFO_STREAM("Running VID-Fusion!");
      }
    }
  }
  SVO_INFO_STREAM("=======================================");

  DynamicsHandler::Ptr dynamics_handler(new DynamicsHandler(options));
  
  return dynamics_handler;
}

void setInitialPose(const rclcpp::Node::SharedPtr& pnh, FrameHandlerBase& vo)
{
  Transformation T_world_imuinit(
        Quaternion(vk::param<double>(pnh, "T_world_imuinit/qw", 1.0),
                   vk::param<double>(pnh, "T_world_imuinit/qx", 0.0),
                   vk::param<double>(pnh, "T_world_imuinit/qy", 0.0),
                   vk::param<double>(pnh, "T_world_imuinit/qz", 0.0)),
        Vector3d(vk::param<double>(pnh, "T_world_imuinit/tx", 0.0),
                 vk::param<double>(pnh, "T_world_imuinit/ty", 0.0),
                 vk::param<double>(pnh, "T_world_imuinit/tz", 0.0)));
  vo.setInitialImuPose(T_world_imuinit);
}


FrameHandlerMono::Ptr makeMono(const rclcpp::Node::SharedPtr& pnh, const CameraBundlePtr& cam)
{
  // Create camera
  CameraBundle::Ptr ncam = (cam) ? cam : loadCameraFromYaml(pnh);
  if (ncam->numCameras() > 1)
  {
    LOG(WARNING) << "Load more cameras than needed, will erase from the end.";
    ncam->keepFirstNCams(1);
  }

  // Init VO
  FrameHandlerMono::Ptr vo =
      std::make_shared<FrameHandlerMono>(
        loadBaseOptions(pnh, false),
        loadDepthFilterOptions(pnh),
        loadDetectorOptions(pnh),
        loadInitializationOptions(pnh),
        loadReprojectorOptions(pnh),
        loadTrackerOptions(pnh),
        ncam);

  // Get initial position and orientation of IMU
  setInitialPose(pnh, *vo);

  return vo;
}

FrameHandlerStereo::Ptr makeStereo(const rclcpp::Node::SharedPtr& pnh, const CameraBundlePtr& cam)
{
  // Load cameras
  CameraBundle::Ptr ncam = (cam) ? cam : loadCameraFromYaml(pnh);
  if (ncam->numCameras() > 2)
  {
    LOG(WARNING) << "Load more cameras than needed, will erase from the end.";
    ncam->keepFirstNCams(2);
  }

  // Init VO
  InitializationOptions init_options = loadInitializationOptions(pnh);
  init_options.init_type = InitializerType::kStereo;
  FrameHandlerStereo::Ptr vo =
      std::make_shared<FrameHandlerStereo>(
        loadBaseOptions(pnh, true),
        loadDepthFilterOptions(pnh),
        loadDetectorOptions(pnh),
        init_options,
        loadStereoOptions(pnh),
        loadReprojectorOptions(pnh),
        loadTrackerOptions(pnh),
        ncam);

  // Get initial position and orientation of IMU
  setInitialPose(pnh, *vo);

  return vo;
}

FrameHandlerArray::Ptr makeArray(const rclcpp::Node::SharedPtr& pnh, const CameraBundlePtr& cam)
{
  // Load cameras
  CameraBundle::Ptr ncam = (cam) ? cam : loadCameraFromYaml(pnh);

  // Init VO
  InitializationOptions init_options = loadInitializationOptions(pnh);
  init_options.init_type = InitializerType::kArrayGeometric;
  init_options.init_min_disparity = 25;
  DepthFilterOptions depth_filter_options = loadDepthFilterOptions(pnh);
  depth_filter_options.verbose = true;
  FrameHandlerArray::Ptr vo =
      std::make_shared<FrameHandlerArray>(
        loadBaseOptions(pnh, true),
        depth_filter_options,
        loadDetectorOptions(pnh),
        init_options,
        loadReprojectorOptions(pnh),
        loadTrackerOptions(pnh),
        ncam);

  // Get initial position and orientation of IMU
  setInitialPose(pnh, *vo);

  return vo;
}

} // namespace factory
} // namespace svo
