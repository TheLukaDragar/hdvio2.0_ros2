#pragma once

#include <cmath>
#include <deque>
#include <iomanip>
#include <iostream>
#include <vector>

#include <Eigen/Core>

#include <svo/vio_common/logging.hpp>


namespace svo {

class QuadAngVelSpline
{
public:
    static constexpr int N_ = 5;        ///< Order of the spline.
    static constexpr int DEG_ = N_ - 1;  ///< Degree of the spline.

    static constexpr double NS_TO_S_ = 1e-9;  ///< Nanosecond to second conversion
    static constexpr double S_TO_NS_ = 1e9;   ///< Second to nanosecond conversion
    
    using MatN = Eigen::Matrix<double, N_, N_>;
    using VecN = Eigen::Matrix<double, N_, 1>;

    using Vec3 = Eigen::Matrix<double, 3, 1>;
    using Mat3 = Eigen::Matrix<double, 3, 3>;

    using aligned_vector = std::vector<Vec3, Eigen::aligned_allocator<Vec3>>;
    using aligned_deque = std::deque<Vec3, Eigen::aligned_allocator<Vec3>>;

    /// @brief Default constructor
    QuadAngVelSpline();

    /// @brief Constructor with knot interval and spline length
    ///
    /// @param[in] time_interval_ns knot time interval in nanoseconds
    /// @param[in] spline_length_ns start time of the spline in nanoseconds
    QuadAngVelSpline(const int64_t time_interval_ns, const int64_t spline_length_ns);

    /// @brief Default destructor
    ~QuadAngVelSpline();

    const double nsec_to_sec(const int64_t t);
    const int64_t sec_to_nsec(const double t);

    /// @brief Struct to store the Jacobian of the spline
    ///
    /// Since B-spline of order N has local support (only N knots infuence the
    /// value) the Jacobian is zero for all knots except maximum N for value and
    /// all derivatives.
    struct JacobianStruct 
    {
        size_t start_idx;                     ///< Start index of the non-zero elements of the Jacobian.
        std::array<double, N_> d_val_d_knot;  ///< Value of nonzero Jacobians.
    };

    /// @brief Struct to store the Jacobian of the spline
    ///
    /// Since B-spline of order N has local support (only N knots infuence the
    /// value) the Jacobian is zero for all knots except maximum N for value and
    /// all derivatives.
    struct JacobianStructRotDynamicsRes
    {
        size_t start_idx;                   ///< Start index of the non-zero elements of the Jacobian.
        std::array<Mat3, N_> d_res_d_knot;  ///< Value of nonzero Jacobians
    };

    /// @brief Get start dt
    ///
    inline int64_t get_dt_ns() const
    {
        return dt_ns_;
    }

    /// @brief Set start time for spline
    ///
    /// @param[in] start_time_ns start time of the spline in nanoseconds
    inline void set_start_time_ns(int64_t start_time_ns) 
    {
        start_t_ns_ = start_time_ns;
    }

    /// @brief Get start time for spline
    ///
    inline int64_t get_start_time_ns() const
    {
        return start_t_ns_;
    }
    
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

    /// @brief Get spline knots
    void set_knot(size_t i, Vec3& vec);

    /// @brief Get spline knots
    void get_knot(size_t i, Vec3& vec) const;

    /// @brief Set spline knots
    ///
    /// @param[in] vector R(DIM) vectors
    void set_knots(aligned_deque& vec, int64_t t_start);

    /// @brief Get spline knots
    void get_knots(aligned_deque& vec) const;

    /// @brief Get spline knots timestamps
    void get_knots_times(std::deque<int64_t>& vec) const;

    /// @brief Get number of knots
    inline size_t get_num_knots() const
    {
        return knots_.size();
    };

    /// @brief Return time interval in nanoseconds
    ///
    /// @return time interval in nanoseconds
    int64_t get_time_interval_ns() const;
    /// @return time interval in seconds
    const double get_time_interval_s();

    /// @brief Set quad's inertia matrix
    ///
    /// @param[in] J inertial matrix
    void set_inertia(Mat3& J);

    /// @brief Evaluate value or derivative of the spline.
    ///
    /// @param derivative derivative to evaluate (0 for value)
    /// @param[in] time_ns time for evaluating of the spline in nanoseconds
    /// @param[out] J if not nullptr, return the Jacobian of the value with
    /// respect to knots
    /// @return value of the spline or derivative. Euclidean vector of dimention 3.
    const Vec3 evaluate(
        const int derivative, int64_t time_ns, JacobianStruct* J = nullptr);

    /// @brief Evaluate angular velocity residual.
    ///
    /// @param[in] time_ns time of the measurement
    /// @param[in] measured_angvel angular velocity measurement
    /// @param[out] Jac if not nullptr, Jacobian with respect to knots of the spline
    /// @return angular velocity residual
    const Vec3 angular_velocity_residual(
        int64_t time_ns, const Vec3 &measured_angvel, JacobianStruct *Jac = nullptr);

    /// @brief Evaluate angular acceleration residual.
    ///
    /// @param[in] time_ns time of the measurement
    /// @param[in] measured_angaccel angular acceleration measurement
    /// @param[out] Jac if not nullptr, Jacobian with respect to knots of the spline
    /// @return angular acceleration residual
    const Vec3 angular_acceleration_residual(
        int64_t time_ns, const Vec3 &measured_angaccel, JacobianStruct *Jac = nullptr);

    /// @brief Evaluate angular velocity residual.
    /// res = J * w_dot + w x J * w - tau (J = inertia matrix, tau = torque) 
    ///
    /// @param[in] time_ns time of the measurement
    /// @param[in] torque_meas torque measurement
    /// @param[out] Jac if not nullptr, Jacobian with respect to knots of the spline
    /// @return rotational dynamics residual
    const Vec3 rot_dynamics_residual(
        int64_t time_ns, const Vec3 &torque_meas, JacobianStructRotDynamicsRes *Jac = nullptr);

    /// @brief Apply increment to the knot
    ///
    /// The incremernt vector to be added to the current knot values.
    /// @param[in] i index of the knot
    /// @param[in] inc 3x1 increment vector
    void apply_inc(const int i, const Vec3 &inc);

protected:
    /// @brief Vector of derivatives of time polynomial.
    ///
    /// Computes a derivative of \f$ \begin{bmatrix}1 & t & t^2 & \dots &
    /// t^{N-1}\end{bmatrix} \f$ with repect to time. For example, the first
    /// derivative would be \f$ \begin{bmatrix}0 & 1 & 2 t & \dots & (N-1)
    /// t^{N-2}\end{bmatrix} \f$.
    /// @param derivative derivative to evaluate
    /// @param[out] res_const vector to store the result
    /// @param[in] t
    const void base_coeffs_with_time(
        const double t, const VecN& res_const, const int derivative);

    MatN blending_matrix_;    ///< Blending matrix. See \ref computeBlendingMatrix.
    MatN base_coefficients_;  ///< Base coefficients matrix. See \ref computeBaseCoefficients.

    aligned_deque knots_;                 ///< Knots
    std::deque<int64_t> knots_times_;     ///< Knot times
    int64_t dt_ns_ = 1e7;                 ///< Knot interval in nanoseconds
    int64_t start_t_ns_ = 0;              ///< Start time in nanoseconds
    int64_t spline_length_ns_ = 1e9;      ///< Spline length
    size_t n_knots_;                      ///< Number of knots
    std::array<double, N_> pow_inv_dt_;   ///< Array with inverse powers of dt

    Mat3 inertia_;  ///< Quad inertia matrix
    Vec3 inertia_delta_; ///< See notes

    bool verbose_ = false;

};

/// @brief Compute skew matrix of a 3-d vector.
///
/// @param[in] v vector in R^3
/// @return skew matrix
QuadAngVelSpline::Mat3 skew(QuadAngVelSpline::Vec3& v);

/// @brief Compute pseudo skew matrix (see notes) of a 3-d vector.
///
/// @param[in] v vector in R^3
/// @return pseudo skew matrix
QuadAngVelSpline::Mat3 pseudo_skew(QuadAngVelSpline::Vec3& v);

/// @brief Compute binomial coefficient.
///
/// Computes number of combinations that include k objects out of n.
/// @param[in] n
/// @param[in] k
/// @return binomial coefficient
const uint64_t binomial_coefficient(uint64_t n, uint64_t k);

/// @brief Compute base coefficient matrix for polynomials of size N.
///
/// In each row starting from 0 contains the derivative coefficients of the
/// polynomial. For _N=5 we get the following matrix: \f[ \begin{bmatrix}
///   1 & 1 & 1 & 1 & 1
/// \\0 & 1 & 2 & 3 & 4
/// \\0 & 0 & 2 & 6 & 12
/// \\0 & 0 & 0 & 6 & 24
/// \\0 & 0 & 0 & 0 & 24
/// \\ \end{bmatrix}
/// \f]
/// Function \ref QuadAngVelSpline::baseCoeffsWithTime uses this matrix to compute derivatives of the
/// time polynomial.
const QuadAngVelSpline::MatN compute_base_coefficients(
    const int order, const int deg);

/// @brief Compute blending matrix for uniform B-spline evaluation.
///
/// @param N order of the spline
/// @param Cumulative if the spline should be cumulative
const QuadAngVelSpline::MatN compute_blending_matrix(
    const int order, const bool cumulative = false);

} // namespace svo