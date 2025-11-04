// This file is part of HDIVO2 - Visual Inertial Odometry ++.
//
// Copyright (C) 2022 Giovanni Cioffi <cioffi at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// This file is subject to the terms and conditions defined in the file
// 'LICENSE', which is part of this source code package.

#pragma once


#include <cassert>
#include <cfloat>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include <cuda_runtime_api.h>
#include <Eigen/Eigen>
#include <NvInfer.h>
#include <NvInferRuntime.h>
#include <NvOnnxParser.h>
#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>


namespace svo {

// destroy TensorRT objects - TensorRT 8+ uses delete instead of destroy()
struct TRTDestroy
{
    template <class T>
    void operator()(T* obj) const
    {
        if (obj)
        {
            delete obj;
        }
    }
};    


template <class T>
using TRTUniquePtr = std::unique_ptr<T, TRTDestroy>;


class DynamicsNet
{
    public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef std::shared_ptr<DynamicsNet> Ptr;
    
    DynamicsNet(const std::string& net_model_file, const std::string& type);
    ~DynamicsNet();

    // Use this function to load a .onnx model and build the engine
    void parseOnnxModel();
    // Use this function if the engine is already available
    void parseTrtEngine();
    void allocateMemory();
    float* inference(float* input);

    private:
    // initialize TensorRT engine and context
    TRTUniquePtr<nvinfer1::ICudaEngine> engine_{nullptr};
    TRTUniquePtr<nvinfer1::IExecutionContext> context_{nullptr};

    std::string net_model_file_;
    std::string type_;
    const int batch_size_ = 1;
    size_t input_size_;
    size_t output_size_;
    // TensorRT 10+ uses name-based tensor API
    const char* input_name_;
    const char* output_name_;
    void* input_buffer_;
    void* output_buffer_;
};

} // namespace svo