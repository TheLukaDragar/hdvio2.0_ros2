// This file is part of HDVIO2 - Visual Inertial Odometry ++.
//
// Copyright (C) 2022 Giovanni Cioffi <cioffi at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// This file is subject to the terms and conditions defined in the file
// 'LICENSE', which is part of this source code package.

#include "svo/dynamics_net.h"

namespace svo 
{

// class to log errors, warnings, and other information during the build and inference phases
class Logger : public nvinfer1::ILogger
{
    void log(Severity severity, const char* msg) noexcept override
    {
        // remove this 'if' if you need more logged info
        if ((severity == Severity::kERROR) || (severity == Severity::kINTERNAL_ERROR)) 
        { std::cout << msg << "\n"; }
    }
} gLogger;


// Function to check for cuda errors
template<typename T>
void checkCuda(T retVal) 
{
    if (retVal != 0) 
    {
        std::cerr << "Cuda failure: " << retVal << std::endl;
        std::cout << "checkCuda failed!";
    }
}


DynamicsNet::DynamicsNet(
    const std::string& net_model_file, const std::string& type)
{
    net_model_file_ = net_model_file;
    type_ = type;
    // call allocateMemory() to initialize
    input_idx_ = 0;
    output_idx_ = 0;
    input_size_ = 0;
    output_size_ = 0;
}


DynamicsNet::~DynamicsNet()
{
    for (void* buf : buffers_)
    {
        cudaFree(buf);
    }
}


void DynamicsNet::parseOnnxModel()
{
    TRTUniquePtr<nvinfer1::IBuilder> builder{nvinfer1::createInferBuilder(gLogger)};
    const auto explicit_batch = 1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
    TRTUniquePtr<nvinfer1::INetworkDefinition> network{builder->createNetworkV2(explicit_batch)};
    TRTUniquePtr<nvonnxparser::IParser> parser{nvonnxparser::createParser(*network, gLogger)};
    TRTUniquePtr<nvinfer1::IBuilderConfig> config{builder->createBuilderConfig()};

    // parse ONNX
    if (!parser->parseFromFile(net_model_file_.c_str(), static_cast<int>(nvinfer1::ILogger::Severity::kINFO)))
    {
        std::cout << "Could not parse the model from file " << net_model_file_ << "\n";
        return;
    }
    
    // allow TensorRT to use up to 1GB of GPU memory for tactic selection.
    // config->setMaxWorkspaceSize(1ULL << 30);
    
    // use FP16 mode if possible
    if (builder->platformHasFastFp16())
    {
        config->setFlag(nvinfer1::BuilderFlag::kFP16);
    }
    
    // generate TensorRT engine optimized for the target platform
    engine_.reset(builder->buildEngineWithConfig(*network, *config));
    context_.reset(engine_->createExecutionContext());
}

void DynamicsNet::parseTrtEngine()
{
    // De-serialize engine from file
    std::ifstream engine_file(net_model_file_, std::ios::binary);
    if (engine_file.fail())
    {
        std::cout << "[" << type_ << " net] De-serializing engine failed!\n";
    }

    // get length of file:
    engine_file.seekg(0, std::ifstream::end);
    auto fsize = engine_file.tellg();
    engine_file.seekg(0, std::ifstream::beg);

    std::vector<char> engine_data(fsize);
    engine_file.read(engine_data.data(), fsize);

    TRTUniquePtr<nvinfer1::IRuntime> runtime{nvinfer1::createInferRuntime(gLogger)};

    // initialize engine
    engine_.reset(runtime->deserializeCudaEngine(engine_data.data(), fsize, nullptr));
    context_.reset(engine_->createExecutionContext());
}


void DynamicsNet::allocateMemory()
{
    std::cout << "[" << type_ << " net] Allocating memory for inference!\n";

    // get sizes of input and output and allocate memory required for input data and for output data
    buffers_.reserve(engine_->getNbBindings());
    input_idx_ = engine_->getBindingIndex("input");
    output_idx_ = engine_->getBindingIndex("output");
    nvinfer1::Dims3 input_dims = static_cast<nvinfer1::Dims3&&>(
        engine_->getBindingDimensions(input_idx_));
    nvinfer1::Dims3 output_dims = static_cast<nvinfer1::Dims3&&>(
        engine_->getBindingDimensions(output_idx_));
    input_size_ = input_dims.d[1] * input_dims.d[2] * sizeof(float);
    output_size_ = output_dims.d[1] * sizeof(float);
    checkCuda(cudaMalloc(&buffers_[input_idx_], input_size_));
    checkCuda(cudaMalloc(&buffers_[output_idx_], output_size_));

    std::cout << "Memory allocated\n";
    std::cout << "Input size = " << input_size_ 
    << " [dim1 (" << input_dims.d[1] << ") * dim2 (" << input_dims.d[2] << ") * sizeoffloat (" << sizeof(float) << ")]\n";
    std::cout << "Output size = " << output_size_ 
    << " [dim1 (" << output_dims.d[1] << ") * sizeoffloat (" << sizeof(float) << ")]\n";
}


float* DynamicsNet::inference(float* input)
{
    // copy inputs to GPU
    checkCuda(
        cudaMemcpy(buffers_[input_idx_], input, input_size_, cudaMemcpyHostToDevice));

    // inference
    context_->executeV2(buffers_.data());

    // copy ouput from GPU
    float* output = nullptr;
    output = new float[15];
    checkCuda(
        cudaMemcpy(output, buffers_[output_idx_], output_size_, cudaMemcpyDeviceToHost));

    return output;  
}

} // namespace svo
