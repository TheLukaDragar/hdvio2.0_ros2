#pragma once

#include <cstddef>
#include <chrono>
#include <memory>
#include <unordered_map>

#include <Eigen/Sparse>

#include <svo/quad_angvel_spline.h>


namespace svo {

class OptimizeQuadAngVelSpline
{
public:
    using Vec3 = Eigen::Matrix<double, 3, 1>;
    using Mat3 = Eigen::Matrix<double, 3, 3>;

    using aligned_vector = std::vector<Vec3, Eigen::aligned_allocator<Vec3>>;
    using aligned_deque = std::deque<Vec3, Eigen::aligned_allocator<Vec3>>;

    /// @brief Struct to store the angular velocity measurements
    struct Measurement
    {
        double time;
        Vec3 value;
    };
    using aligned_deque_meas = std::deque<Measurement, Eigen::aligned_allocator<Measurement>>;

    typedef std::shared_ptr<OptimizeQuadAngVelSpline> Ptr;

    /// @brief Default constructor
    OptimizeQuadAngVelSpline();
    
    /// @brief Constructor with knot interval ad spline length
    ///
    /// @param[in] dt_ns knot time interval in nanoseconds
    /// @param[in] spline_length_ns spline length in nanoseconds
    OptimizeQuadAngVelSpline(const int64_t dt_ns, const int64_t spline_length_ns);
    
    /// @brief Default destructor
    ~OptimizeQuadAngVelSpline();

    const double nsec_to_sec(const int64_t t);
    const int64_t sec_to_nsec(const double t);

    /// @brief Get dt in nanoseconds
    ///
    const int64_t get_dt_ns();

    /// @brief Get dt in seconds
    ///
    const double get_dt_s();

    /// @brief Set start time for spline in nanoseconds
    ///
    /// @param[in] start_time_ns start time of the spline in nanoseconds
    void set_start_time_ns(const int64_t start_time_ns);

    /// @brief Set start time for spline in seconds
    ///
    /// @param[in] start_time_s start time of the spline in seconds
    void set_start_time_s(const double start_time_s);

    /// @brief Get start time for spline in nanoseconds
    ///
    const int64_t get_start_time_ns();

    /// @brief Get start time for spline in seconds
    ///
    const double get_start_time_s();

    /// @brief Maximum time represented by spline
    ///
    /// @return maximum time represented by spline in nanoseconds
    int64_t max_time_ns() const;
    /// @return maximum time represented by spline in seconds
    const double max_time_s();

    /// @brief Minimum time represented by spline
    ///
    /// @return minimum time represented by spline in nanoseconds
    int64_t min_time_ns() const;
    /// @return minimum time represented by spline in seconds
    const double min_time_s();

    /// @brief Set spline knots
    ///
    /// @param[in] vector R(3) vectors
    void set_knots(aligned_deque& vec, int64_t t_start);

    /// @brief Get Spline order
    int get_order() const;

    /// @brief Get Spline degree
    int get_degree() const;
    
    /// @brief Get knot
    void get_knot(size_t i, Vec3& knot) const;
    
    /// @brief Get knots
    void get_knots(aligned_deque& knots) const;

    /// @brief Get knots timestamps
    void get_knots_times(std::deque<int64_t>& vec) const;

    /// @brief Get number of measurements 
    /// @param[in] mode = 0: ang vel residuals, 1: ang accel residuals, 2: ang vel residuals and rot dynamics residuals
    size_t get_num_of_measurements(int mode) const;

    /// @brief Get number of knots
    const size_t get_num_knots() const;

    /// @brief Set quad's inertia matrix
    ///
    /// @param[in] J inertial matrix
    void set_inertia(Mat3& J);

    // @brief Sample value or derivative of the spline.
    ///
    /// @param derivative derivative to evaluate (0 for value)
    /// @param[in] time_ns time for evaluating of the spline in nanoseconds
    /// @return value of the spline or derivative. Euclidean vector of dimention 3.
    void sample(
        Vec3& val, const int derivative, int64_t time_ns);

    /// @brief Add angular velocity measurements
    void add_ang_vel_measurements(
        std::vector<double>& times,
        aligned_vector& ang_vel_measurements);

    /// @brief Add angular acceleration measurements
    void add_ang_accel_measurements(
        std::vector<double>& times,
        aligned_vector& ang_accel_measurements);

    /// @brief Add rotational dynamcis measurements
    void add_rot_dynamics_measurements(
        std::vector<double>& times,
        aligned_vector& rot_dynamics_measurements);

    /// @brief Compute angular velocity errors
    ///
    /// @param[out] error
    double compute_ang_vel_errors(bool compute_jacobian);

    /// @brief Compute angular acceleration errors
    ///
    /// @param[out] error
    double compute_ang_accel_errors(bool compute_jacobian);

    /// @brief Compute rotational dynamics errors
    ///
    /// @param[out] error
    double compute_rot_dynamics_errors(bool compute_jacobian);

    /// @brief Optimize spline
    ///
    /// @param[in] stop_thresh
    /// @param[in] error
    /// @param[in] mode = 0: ang vel residuals, 1: ang accel residuals, 2: ang vel residuals and rot dynamics residuals
    /// @param[in] print_info
    /// @param[out] converged
    bool optimize(
        double stop_thresh, double& error, int mode, bool print_info = true);

    /// @brief Apply optimization result
    void apply_inc(const Eigen::VectorXd& inc_full);

    /// @brief Function to test stuff
    void test();

private:

    QuadAngVelSpline spline_;

    // optimization-related parameters
    double lambda_ = 1e-6;
    const double min_lambda_ = 1e-18;
    const double max_lambda_ = 100.0;
    double lambda_vee_ = 2.0;
    const double ang_vel_std_ = 10.0;
    const double ang_vel_var_inv_ = 1.0 / (ang_vel_std_ * ang_vel_std_);
    const double ang_accel_std_ = 10.0;
    const double ang_accel_var_inv_ = 1.0 / (ang_accel_std_ * ang_accel_std_);
    const double torque_std_ = 1.0;
    const double torque_var_inv_ = 1.0 / (torque_std_ * torque_std_);
    size_t opt_problem_size_;
    bool iterative_solver_ = false;
    
    // accumulator of weighted squared norm residuals
    double error_ = 0.0;

    aligned_deque_meas ang_vel_measurements_;
    aligned_deque_meas ang_vel_measurements_buffer_;
    aligned_deque_meas ang_accel_measurements_;
    aligned_deque_meas ang_accel_measurements_buffer_;
    aligned_deque_meas torque_measurements_;
    aligned_deque_meas torque_measurements_buffer_;

    const int64_t min_time_us_ = std::numeric_limits<int64_t>::max();
    const int64_t max_time_us_ = std::numeric_limits<int64_t>::min();

    // hash map to store non-zero hessian (= Jac^t*Jac) entries 
    using Key = std::array<int, 4>;
    struct KeyHash 
    {
        inline size_t operator()(const Key& c) const 
        {
            size_t seed = 0;
            for (int i = 0; i < 4; i++) 
            {
                seed ^= std::hash<int>{}(c[i]) + 0x9e3779b97f4a7c15LLU + (seed << 12) + (seed >> 4);
            }
            return seed;
        }
    };
    std::unordered_map<Key, Mat3, KeyHash> hash_map_;

    /// @todo can I avoid using a dynamic matrix here ?
    // b /in R(3 x knots_.size())
    // Opt. problem: 
    // (Jac^t * Jac + lambda*diag(Jac^t * Jac)) = Jac^t * res ==>
    // ==> (H + lambda*diag(H)) = b
    Eigen::Matrix<double, Eigen::Dynamic, 1> b_;
    Eigen::SparseMatrix<double> smm_;

    bool verbose_ = false;
};

} // namespace svo