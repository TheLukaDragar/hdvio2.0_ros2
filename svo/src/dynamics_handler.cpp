#include "svo/dynamics_handler.h"


namespace svo {

DynamicsHandler::DynamicsHandler(const DynamicsHandlerOptions& options)
  : options_(options) 
  {
    thrust_dt_net_in_ = 1.0 / options_.thrust_net_sampling_freq;
    thrust_buffer_size_net_in_ = static_cast<int>(options_.thrust_net_sampling_freq * options_.thrust_net_input_window_length);

    torque_dt_net_in_ = 1.0 / options_.torque_net_sampling_freq;
    torque_buffer_size_net_in_ = static_cast<int>(options_.torque_net_sampling_freq * options_.torque_net_input_window_length);

    dt_net_in_ = thrust_dt_net_in_;
    buffer_size_net_in_ = thrust_buffer_size_net_in_;

    if (options.use_rotational_dynamics)
    {
        if (torque_dt_net_in_ < thrust_dt_net_in_)
        {
            dt_net_in_ = torque_dt_net_in_;
        }

        if (torque_buffer_size_net_in_ > thrust_buffer_size_net_in_)
        {
            buffer_size_net_in_ = torque_buffer_size_net_in_;
        }
    }

    net_inputs_.set_capacity(buffer_size_net_in_);

    SVO_INFO_STREAM("Dynamic network input dt = " << dt_net_in_);
    SVO_INFO_STREAM("Dynamic network input buffer size = " << buffer_size_net_in_);
  }

DynamicsHandler::~DynamicsHandler(){}

bool DynamicsHandler::addDynamicsMeasurement(const DynamicsMeasurement& m)
{
    ulock_t lock(measurements_mut_);
    measurements_.push_front(m); // new measurement is at the front of the list!
    return true;
}

bool DynamicsHandler::addNetInputs(const DynamicsMeasurements& m)
{
    for(auto it=m.rbegin(); it!=m.rend(); ++it)
    {
        bool add_measurement = true;

        if (net_inputs_.size() > 0)
        {
            if ((it->timestamp_ - net_inputs_.back().timestamp_) < dt_net_in_)
            {
                add_measurement = false;
            }
        }

        if (add_measurement)
        {
            DynamicsMeasurement in_meas;
            in_meas.timestamp_ = it->timestamp_;
            in_meas.gyro_ = it->gyro_;
            in_meas.collective_thrust_ = it->collective_thrust_;
            in_meas.collective_torque_ = it->collective_torque_;
            net_inputs_.push_back(in_meas);
        }
    }
    
    return true;
}
  
bool DynamicsHandler::getNetInputs(std::deque<DynamicsMeasurement>& m, const std::string& type)
{
    int net_input_size = static_cast<int>(net_inputs_.size());

    if (net_input_size == buffer_size_net_in_)
    {
        int n = 0;
        if (type == "thrust")
        {
            n = thrust_buffer_size_net_in_;
        }
        else if (type == "torque")
        {
            n = torque_buffer_size_net_in_;
        }
        else
        {
            return false;
        }
        
        for (int i = 0; i < n; i++)
        {
            int idx = buffer_size_net_in_ - 1 - i;

            DynamicsMeasurement mi;
            mi.timestamp_ = net_inputs_[idx].timestamp_;
            mi.gyro_ = net_inputs_[idx].gyro_;
            if (type == "thrust")
            {
                mi.collective_thrust_ = net_inputs_[idx].collective_thrust_;
            }
            else if (type == "torque")
            {
                mi.collective_torque_ = net_inputs_[idx].collective_torque_;
            }

            m.push_front(mi);
        }

        return true;
    }
    else
    {
        return false;
    }
}

int DynamicsHandler::getNetInBufferSize()
{
    return buffer_size_net_in_;
}

int DynamicsHandler::getThrustNetInBufferSize()
{
    return thrust_buffer_size_net_in_;
}

int DynamicsHandler::getTorqueNetInBufferSize()
{
    return torque_buffer_size_net_in_;
}

bool DynamicsHandler::getMeasurementsContainingEdges(
    const double frame_timestamp, DynamicsMeasurements& extracted_measurements, const bool remove_measurements)
{
    ulock_t lock(measurements_mut_);
    if(measurements_.empty())
    {
        VLOG(0) << "Dynamics measurements vector is empty!";
        return false;
    }

    const double t = frame_timestamp;
    
    // Find the first measurement newer than frame_timestamp,
    // note that the newest measurement is at the front of the list!
    DynamicsMeasurements::iterator it=measurements_.begin();
    for(; it!=measurements_.end(); ++it)
    {
        if(it->timestamp_ < t)
        {
            if(it == measurements_.begin())
            {
                return false;
            }
            //decrement iterator again to point to element >= t
            --it;
            break;
        }
    }

    // copy affected measurements
    extracted_measurements.insert(extracted_measurements.begin(), it, measurements_.end());

    // check
    if(extracted_measurements.size()<2)
    {
        VLOG(0) << "Extracted only " << extracted_measurements.size() << " dynamics measurements!";
        extracted_measurements.clear();
        return false;
    }

    if (remove_measurements)
    {
        // delete measurements that will not be used anymore (such that we keep it+1,
        // the first frame with smaller timestamp (earlier) than frame_timestamp,
        // which will be used in interpolation in next iteration
        measurements_.erase(it+2, measurements_.end());
    }

    return true;
}

bool DynamicsHandler::getMeasurementsExcludingEdges(const double t0, const double t1, 
DynamicsMeasurements& extracted_measurements, const bool remove_measurements)
{
    ulock_t lock(measurements_mut_);
    if(measurements_.empty())
    {
        VLOG(0) << "Dynamics measurements vector is empty!";
        return false;
    }

    if (measurements_.front().timestamp_ < t0)
    {
        VLOG(0) << "Dynamics measurements too old!";
        return false;
    }

    // Remember: t1 > t0
    // Find the first measurement older than t1,
    // note that the newest measurement is at the front of the list!
    DynamicsMeasurements::iterator it1=measurements_.begin();
    for(; it1!=measurements_.end(); ++it1)
    {
        if(it1->timestamp_ < t1)
        {
            break;
        }
    }

    if (it1 == measurements_.end())
    {
        VLOG(0) << "Extract dynamics measurements: it1 is at the end of measurements_!";
        return false;
    }

    // Find the first measurement newer than t0.
    DynamicsMeasurements::iterator it0;
    if (measurements_.back().timestamp_ > t0)
    {
        it0 = measurements_.end();
    }
    else
    {
        it0 = it1;
        for(; it0!=measurements_.end(); ++it0)
        {
            if(it0->timestamp_ < t0)
            {
                break;
            }
        }
    }

    // copy affected measurements. it0 is not included
    extracted_measurements.insert(extracted_measurements.begin(), it1, it0);

    const int log_map_verbose_level = 300;
    if (FLAGS_v >= log_map_verbose_level)
    {
        VLOG(log_map_verbose_level) << "imu ts = " << std::setprecision(15) << t0 << ", tf = " << std::setprecision(15) << t1;
        VLOG(log_map_verbose_level) << "dynamics measurement";
        size_t m = measurements_.size();
        for (size_t i = 0; i < m; i++)
        {
            VLOG(log_map_verbose_level) << "t_" << i << " = " << std::setprecision(15) << measurements_.at(i).timestamp_ << "\n";
        }
        VLOG(log_map_verbose_level) << "Extracted dynamics measurement";
        size_t n = extracted_measurements.size();
        VLOG(log_map_verbose_level) << "ts = " << std::setprecision(15) << extracted_measurements.back().timestamp_ << "\n";
        VLOG(log_map_verbose_level) << "tf = " << std::setprecision(15) << extracted_measurements.front().timestamp_ << "\n";
    }

    // check
    if(extracted_measurements.size()<2)
    {
        VLOG(0) << "Extracted only " << extracted_measurements.size() << " dynamics measurements!";
        extracted_measurements.clear();
        return false;
    }

    if (remove_measurements)
    {
        measurements_.erase(it1, measurements_.end());
    }

    return true;
}

bool DynamicsHandler::sampleGyroscopeMeasurements(
    const ImuMeasurements& imu_measurements, DynamicsMeasurements& dynamics_measurements)
{
    // Check that oldest imu measurement is older than oldest thrust measurement
    if (imu_measurements.back().timestamp_ > dynamics_measurements.back().timestamp_)
    {
        VLOG(0) << "Oldest imu measurement is newer than oldest thrust measurement!";
        return false;
    }
    
    // Check that newest imu measurement is newer than newest thrust measurement
    if (imu_measurements.front().timestamp_ < dynamics_measurements.front().timestamp_)
    {
        VLOG(0) << "Newest imu measurement is older than newest thrust measurement!";
        return false;
    }

    const size_t n_imu = imu_measurements.size();
    const size_t n_dynamics = dynamics_measurements.size();
    size_t j = 0;

    for (size_t i = 0; i < n_dynamics; i++)
    {
        double t_i = dynamics_measurements.at(i).timestamp_;

        double t_j0 = imu_measurements.at(j).timestamp_;
        while (t_j0 > t_i)
        {
            j++;
            if (j == n_imu)
            {
                VLOG(0) << "Exiting gyro interpolation because: j == n_imu!";
                return false;
            }
            t_j0 = imu_measurements.at(j).timestamp_;
        }

        // perform interpolation i
        if (j == 0)
        {
            VLOG(0) << "Exiting gyro interpolation because: j == 0!";
            return false;
        }

        double t_j1 = imu_measurements.at(j-1).timestamp_;
        Eigen::Vector3d w_j0 = imu_measurements.at(j).angular_velocity_;
        Eigen::Vector3d w_j1 = imu_measurements.at(j-1).angular_velocity_;
        Eigen::Vector3d w_i(Eigen::Vector3d::Zero());

        double c = (t_i - t_j0) / (t_j1 - t_j0);
        if (! ((c > 0.001 ) && (c <= 1.0)) )
        {
            VLOG(0) << "Exiting gyro interpolation because interpolation coeff. is " << c;
            return false;
        }
        w_i = c * (w_j1 - w_j0) + w_j0;

        dynamics_measurements.at(i).gyro_ = w_i;
    }

    return true;
}

bool DynamicsHandler::sampleAccelerometerMeasurements(
    const ImuMeasurements& imu_measurements, DynamicsMeasurements& dynamics_measurements)
{
    // Check that oldest imu measurement is older than oldest thrust measurement
    if (imu_measurements.back().timestamp_ > dynamics_measurements.back().timestamp_)
    {
        VLOG(0) << "Oldest imu measurement is newer than oldest thrust measurement!";
        return false;
    }
    
    // Check that newest imu measurement is newer than newest thrust measurement
    if (imu_measurements.front().timestamp_ < dynamics_measurements.front().timestamp_)
    {
        VLOG(0) << "Newest imu measurement is older than newest thrust measurement!";
        return false;
    }

    const size_t n_imu = imu_measurements.size();
    const size_t n_dynamics = dynamics_measurements.size();
    size_t j = 0;

    for (size_t i = 0; i < n_dynamics; i++)
    {
        double t_i = dynamics_measurements.at(i).timestamp_;

        double t_j0 = imu_measurements.at(j).timestamp_;
        while (t_j0 > t_i)
        {
            j++;
            if (j == n_imu)
            {
                VLOG(100) << "Exiting accel interpolation because: j == n_imu!";
                return false;
            }
            t_j0 = imu_measurements.at(j).timestamp_;
        }

        // perform interpolation i
        if (j == 0)
        {
            VLOG(100) << "Exiting accel interpolation because: j == 0!";
            return false;
        }

        double t_j1 = imu_measurements.at(j-1).timestamp_;
        Eigen::Vector3d a_j0 = imu_measurements.at(j).linear_acceleration_;
        Eigen::Vector3d a_j1 = imu_measurements.at(j-1).linear_acceleration_;
        Eigen::Vector3d a_i(Eigen::Vector3d::Zero());

        double c = (t_i - t_j0) / (t_j1 - t_j0);
        if (! ((c > 0.001 ) && (c <= 1.0)) )
        {
            VLOG(100) << "Exiting gyro interpolation because interpolation coeff. is " << c;
            return false;
        }
        a_i = c * (a_j1 - a_j0) + a_j0;

        dynamics_measurements.at(i).accel_ = a_i;
    }

    return true;
}

bool DynamicsHandler::sampleImuMeasurements(
    const ImuMeasurements& imu_measurements, DynamicsMeasurements& dynamics_measurements)
{
    // Check that oldest imu measurement is older than oldest thrust measurement
    if (imu_measurements.back().timestamp_ > dynamics_measurements.back().timestamp_)
    {
        VLOG(0) << "Oldest imu measurement is newer than oldest thrust measurement!";
        return false;
    }
    
    // Check that newest imu measurement is newer than newest thrust measurement
    if (imu_measurements.front().timestamp_ < dynamics_measurements.front().timestamp_)
    {
        VLOG(0) << "Newest imu measurement is older than newest thrust measurement!";
        return false;
    }

    const size_t n_imu = imu_measurements.size();
    const size_t n_dynamics = dynamics_measurements.size();
    size_t j = 0;

    for (size_t i = 0; i < n_dynamics; i++)
    {
        double t_i = dynamics_measurements.at(i).timestamp_;

        double t_j0 = imu_measurements.at(j).timestamp_;
        while (t_j0 > t_i)
        {
            j++;
            if (j == n_imu)
            {
                VLOG(100) << "Exiting gyro interpolation because: j == n_imu!";
                return false;
            }
            t_j0 = imu_measurements.at(j).timestamp_;
        }

        // perform interpolation i
        if (j == 0)
        {
            VLOG(100) << "Exiting gyro interpolation because: j == 0!";
            return false;
        }

        double t_j1 = imu_measurements.at(j-1).timestamp_;
        
        Eigen::Vector3d w_j0 = imu_measurements.at(j).angular_velocity_;
        Eigen::Vector3d w_j1 = imu_measurements.at(j-1).angular_velocity_;
        Eigen::Vector3d w_i(Eigen::Vector3d::Zero());

        Eigen::Vector3d a_j0 = imu_measurements.at(j).linear_acceleration_;
        Eigen::Vector3d a_j1 = imu_measurements.at(j-1).linear_acceleration_;
        Eigen::Vector3d a_i(Eigen::Vector3d::Zero());

        double c = (t_i - t_j0) / (t_j1 - t_j0);
        if (! ((c > 0.001 ) && (c <= 1.0)) )
        {
            VLOG(100) << "Exiting imu interpolation because interpolation coeff. is " << c;
            return false;
        }
        w_i = c * (w_j1 - w_j0) + w_j0;
        a_i = c * (a_j1 - a_j0) + a_j0;

        dynamics_measurements.at(i).gyro_ = w_i;
        dynamics_measurements.at(i).accel_ = a_i;
    }

    return true;
}

bool DynamicsHandler::duplicateBorders(
    const ImuMeasurements& imu_measurements, 
    DynamicsMeasurements& dynamics_measurements)
{
    // Duplicate the dynamics measurements at initial and final imu time
    // This is necessary for integration in the ceres error function
    const Eigen::Vector3d wb0(
        imu_measurements.back().angular_velocity_.x(),
        imu_measurements.back().angular_velocity_.y(),
        imu_measurements.back().angular_velocity_.z()
    );
    const Eigen::Vector3d ab0(
        imu_measurements.back().linear_acceleration_.x(),
        imu_measurements.back().linear_acceleration_.y(),
        imu_measurements.back().linear_acceleration_.z()
    );
    const Eigen::Vector3d th0(
        dynamics_measurements.back().collective_thrust_.x(),
        dynamics_measurements.back().collective_thrust_.y(),
        dynamics_measurements.back().collective_thrust_.z()
    );
    const double t0 = imu_measurements.back().timestamp_;
    DynamicsMeasurement m0;
    m0.timestamp_ = t0;
    m0.gyro_ = wb0;
    m0.body_rates_ = wb0;
    m0.collective_thrust_ = th0;
    m0.accel_ = ab0;

    dynamics_measurements.push_back(m0);

    const Eigen::Vector3d wb1(
        imu_measurements.front().angular_velocity_.x(),
        imu_measurements.front().angular_velocity_.y(),
        imu_measurements.front().angular_velocity_.z()
    );
    const Eigen::Vector3d ab1(
        imu_measurements.front().linear_acceleration_.x(),
        imu_measurements.front().linear_acceleration_.y(),
        imu_measurements.front().linear_acceleration_.z()
    );
    const Eigen::Vector3d th1(
        dynamics_measurements.front().collective_thrust_.x(),
        dynamics_measurements.front().collective_thrust_.y(),
        dynamics_measurements.front().collective_thrust_.z()
    );
    const double t1 = imu_measurements.front().timestamp_;
    DynamicsMeasurement m1;
    m1.timestamp_ = t1;
    m1.gyro_ = wb1;
    m1.body_rates_ = wb1;
    m1.collective_thrust_ = th1;
    m1.accel_ = ab1;
    
    dynamics_measurements.push_front(m1);

    return true;
}

bool DynamicsHandler::loadDynamicsMeasurementsFromFile(
    const std::string& filename, 
    const int dataset_id,
    const Eigen::Matrix3d& R_B_I)
{
    ulock_t lock(measurements_mut_);
    std::ifstream fs(filename.c_str());
    if(!fs.is_open())
    {
        LOG(WARNING) << "Could not open dynamics measurements file: " << filename;
        return false;
    }

    // add all messages to handler
    size_t n = 0;
    while(fs.good() && !fs.eof())
    {
        if(fs.peek() == '#') // skip comments
        fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        double ts = 0.0;
        Eigen::Vector3d mass_norm_collective_thrust(Eigen::Vector3d::Zero());
        Eigen::Vector3d mass_norm_collective_thrust_in_imu_frame(Eigen::Vector3d::Zero());
        Eigen::Vector3d collective_torque(Eigen::Vector3d::Zero());
        Eigen::Vector3d collective_torque_in_imu_frame(Eigen::Vector3d::Zero());

        // Blackbird
        if (dataset_id == 1)
        {
            double stamp, w0, w1, w2, w3;
            fs >> stamp >> w0 >> w1 >> w2 >> w3;

            double omega2_br, omega2_fr, omega2_bl, omega2_fl;
            omega2_br = pow(w3, 2);
            omega2_fr = pow(w1, 2);
            omega2_bl = pow(w2, 2);
            omega2_fl = pow(w0, 2);
            
            // mass-normalized collective thrust
            double fz, fz_br, fz_fr, fz_bl, fz_fl;
            fz_br = options_.thrust_coeffs(3) * omega2_br;
            fz_fr = options_.thrust_coeffs(1) * omega2_fr;
            fz_bl = options_.thrust_coeffs(2) * omega2_bl;
            fz_fl = options_.thrust_coeffs(0) * omega2_fl;

            fz = fz_bl + fz_br + fz_fl + fz_fr;
            fz /= options_.mass;

            // z-axis is pointing downwards
            mass_norm_collective_thrust(2) = -1. * fz;
            mass_norm_collective_thrust_in_imu_frame = mass_norm_collective_thrust;

            // collective torque
            // br = motor 4, fr = motor 2, bl = motor 3, fl = motor 1
            // c_t x omega_i^2 component
            double tau_br, tau_fr, tau_bl, tau_fl;
            tau_br = options_.torque_coeffs(3) * omega2_br;
            tau_fr = options_.torque_coeffs(1) * omega2_fr;
            tau_bl = options_.torque_coeffs(2) * omega2_bl;
            tau_fl = options_.torque_coeffs(0) * omega2_fl;
            
            collective_torque += Eigen::Vector3d(0., 0., tau_fr);
            collective_torque += Eigen::Vector3d(0., 0., tau_bl);
            collective_torque -= Eigen::Vector3d(0., 0., tau_br);
            collective_torque -= Eigen::Vector3d(0., 0., tau_fl);

            // r_i x thrust_i component
            // -1. it's because the z-axis points down
            collective_torque += skew(options_.r_br) * Eigen::Vector3d(0., 0., -1.*fz_br);
            collective_torque += skew(options_.r_fr) * Eigen::Vector3d(0., 0., -1.*fz_fr);
            collective_torque += skew(options_.r_bl) * Eigen::Vector3d(0., 0., -1.*fz_bl);
            collective_torque += skew(options_.r_fl) * Eigen::Vector3d(0., 0., -1.*fz_fl);

            // Transform to IMU frame
            collective_torque_in_imu_frame = R_B_I.inverse() * collective_torque;

            ts = stamp;
        }
        // VID
        else if (dataset_id == 2)
        {
            double stamp, w1, w2, w3, w4;
            fs >> stamp >> w1 >> w2 >> w3 >> w4;

            double omega2_br, omega2_fr, omega2_bl, omega2_fl;
            omega2_br = pow(w4, 2);
            omega2_fr = pow(w1, 2);
            omega2_bl = pow(w3, 2);
            omega2_fl = pow(w2, 2);
            
            // mass-normalized collective thrust
            double fz, fz_br, fz_fr, fz_bl, fz_fl;
            fz_br = options_.thrust_coeffs(3) * omega2_br;
            fz_fr = options_.thrust_coeffs(0) * omega2_fr;
            fz_bl = options_.thrust_coeffs(2) * omega2_bl;
            fz_fl = options_.thrust_coeffs(1) * omega2_fl;

            fz = fz_bl + fz_br + fz_fl + fz_fr;
            fz /= options_.mass;

            mass_norm_collective_thrust(2) = fz;
            mass_norm_collective_thrust_in_imu_frame = mass_norm_collective_thrust;

            // collective torque
            // br = motor 4, fr = motor 2, bl = motor 3, fl = motor 1
            // c_t x omega_i^2 component
            double tau_br, tau_fr, tau_bl, tau_fl;
            tau_br = options_.torque_coeffs(3) * omega2_br;
            tau_fr = options_.torque_coeffs(0) * omega2_fr;
            tau_bl = options_.torque_coeffs(2) * omega2_bl;
            tau_fl = options_.torque_coeffs(1) * omega2_fl;
            
            collective_torque += Eigen::Vector3d(0., 0., tau_fr);
            collective_torque += Eigen::Vector3d(0., 0., tau_bl);
            collective_torque -= Eigen::Vector3d(0., 0., tau_br);
            collective_torque -= Eigen::Vector3d(0., 0., tau_fl);

            // r_i x thrust_i component
            collective_torque += skew(options_.r_br) * Eigen::Vector3d(0., 0., fz_br);
            collective_torque += skew(options_.r_fr) * Eigen::Vector3d(0., 0., fz_fr);
            collective_torque += skew(options_.r_bl) * Eigen::Vector3d(0., 0., fz_bl);
            collective_torque += skew(options_.r_fl) * Eigen::Vector3d(0., 0., fz_fl);

            // Transform to IMU frame
            collective_torque_in_imu_frame = R_B_I.inverse() * collective_torque;

            ts = stamp;
        }
        // Agilicious Drone
        else if (dataset_id == 0)
        {
            double stamp, w1, w2, w3, w4;
            fs >> stamp >> w1 >> w2 >> w3 >> w4;

            double omega2_1, omega2_2, omega2_3, omega2_4;
            omega2_1 = pow(w1, 2);
            omega2_2 = pow(w2, 2);
            omega2_3 = pow(w3, 2);
            omega2_4 = pow(w4, 2);
            
            // mass-normalized collective thrust
            double fz, fz_1, fz_2, fz_3, fz_4;
            fz_1 = options_.thrust_coeffs(0) * omega2_1;
            fz_2 = options_.thrust_coeffs(1) * omega2_2;
            fz_3 = options_.thrust_coeffs(2) * omega2_3;
            fz_4 = options_.thrust_coeffs(3) * omega2_4;

            fz = fz_1 + fz_2 + fz_3 + fz_4;
            fz /= options_.mass;

            mass_norm_collective_thrust(2) = fz;
            mass_norm_collective_thrust_in_imu_frame = R_B_I.inverse() * mass_norm_collective_thrust;

            // collective torque
            // c_t x omega_i^2 component
            double tau_1, tau_2, tau_3, tau_4;
            tau_1 = options_.torque_coeffs(0) * omega2_1;
            tau_2 = options_.torque_coeffs(1) * omega2_2;
            tau_3 = options_.torque_coeffs(2) * omega2_3;
            tau_4 = options_.torque_coeffs(3) * omega2_4;
            
            collective_torque += Eigen::Vector3d(0., 0., tau_3);
            collective_torque += Eigen::Vector3d(0., 0., tau_4);
            collective_torque -= Eigen::Vector3d(0., 0., tau_1);
            collective_torque -= Eigen::Vector3d(0., 0., tau_2);

            // r_i x thrust_i component
            collective_torque += skew(options_.r_br) * Eigen::Vector3d(0., 0., fz_1);
            collective_torque += skew(options_.r_fl) * Eigen::Vector3d(0., 0., fz_2);
            collective_torque += skew(options_.r_bl) * Eigen::Vector3d(0., 0., fz_3);
            collective_torque += skew(options_.r_fr) * Eigen::Vector3d(0., 0., fz_4);

            // Transform to IMU frame
            collective_torque_in_imu_frame = R_B_I.inverse() * collective_torque;

            ts = stamp;
        }
        else
        {
            std::cout << "Dataset " << dataset_id << " not implemeted!\n";
            return false;
        }

        DynamicsMeasurement m;
        m.timestamp_ = ts;
        m.collective_thrust_ = mass_norm_collective_thrust_in_imu_frame;
        m.collective_torque_ = collective_torque_in_imu_frame;
        measurements_.push_front(m);
        n++;
    }

    VLOG(2) << "DynamicsHandler: Loaded " << n << " measurements.";
    return true;
}

} // namespace svo