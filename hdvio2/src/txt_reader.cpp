#include <hdvio2/txt_reader.h>

namespace svo {

bool loadStampedPoses(const std::string& filename, PoseMeasurements& poses)
{
    std::ifstream fs(filename.c_str());
    if(!fs.is_open())
    {
        LOG(WARNING) << "Could not open file: " << filename;
        return false;
    }

    // Read measurements
    size_t n = 0;
    while(fs.good() && !fs.eof())
    {
        if(fs.peek() == '#') // skip comments
        fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        double stamp, tx, ty, tz, qw, qx, qy, qz;
        fs >> stamp >> tx >> ty >> tz >> qx >> qy >> qz >> qw;

        const Eigen::Vector3d p(tx, ty, tz);
        const Eigen::Quaterniond q(qw, qx, qy, qz);
        
        Transformation T(p, q);
        T.getRotation().normalize();

        PoseMeasurement pose(stamp, T);
        poses.push_back(pose);
        
        n++;
    }

    // ToDo: why does the for loop above read twice the last line?
    poses.pop_back();

    LOG(INFO) << "Loaded " << n << " groundtruth samples.";
    return true;
}

} // namespace svo