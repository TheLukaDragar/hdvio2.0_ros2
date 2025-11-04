#pragma once

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <svo/common/camera_fwd.h>

namespace svo {

// forward declarations
class ImuHandler;
class DynamicsHandler;
class FrameHandlerMono;
class FrameHandlerStereo;
class FrameHandlerArray;
class FrameHandlerDenseMono;

namespace factory {

/// Get IMU Handler.
std::shared_ptr<ImuHandler> getImuHandler(
    const rclcpp::Node::SharedPtr& pnh);

/// Get Dynamics Handler.
std::shared_ptr<DynamicsHandler> getDynamicsHandler(
    const rclcpp::Node::SharedPtr& pnh);

/// Factory for Mono-SVO.
std::shared_ptr<FrameHandlerMono> makeMono(
    const rclcpp::Node::SharedPtr& pnh,
    const CameraBundlePtr& cam = nullptr);

/// Factory for Stereo-SVO.
std::shared_ptr<FrameHandlerStereo> makeStereo(
    const rclcpp::Node::SharedPtr& pnh,
    const CameraBundlePtr& cam = nullptr);

/// Factory for Camera-Array-SVO.
std::shared_ptr<FrameHandlerArray> makeArray(
    const rclcpp::Node::SharedPtr& pnh,
    const CameraBundlePtr& cam = nullptr);

/// Factory for Camera-Array-SVO
std::shared_ptr<FrameHandlerDenseMono> makeDenseMono(
    const rclcpp::Node::SharedPtr& pnh);

} // namespace factory
} // namespace mono
