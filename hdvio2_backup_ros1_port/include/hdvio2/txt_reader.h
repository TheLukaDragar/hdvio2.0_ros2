#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include <glog/logging.h>

#include <svo/common/types.h>
#include <svo/common/transformation.h>


namespace svo {

bool loadStampedPoses(const std::string& filename, PoseMeasurements& poses);

} // namespace svo
