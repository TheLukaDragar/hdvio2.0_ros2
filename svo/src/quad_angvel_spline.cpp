#include <svo/quad_angvel_spline.h>

namespace svo {

    QuadAngVelSpline::QuadAngVelSpline(){}

    QuadAngVelSpline::QuadAngVelSpline(const int64_t time_interval_ns, const int64_t spline_length_ns) : 
    dt_ns_(time_interval_ns), spline_length_ns_(spline_length_ns)
    {
        pow_inv_dt_[0] = 1.0;
        pow_inv_dt_[1] = S_TO_NS_ / dt_ns_;

        for (size_t i = 2; i < N_; i++) 
        {
            pow_inv_dt_[i] = pow_inv_dt_[i - 1] * pow_inv_dt_[1];
        }

        base_coefficients_ = compute_base_coefficients(N_, DEG_);
        blending_matrix_ = compute_blending_matrix(N_);

        n_knots_ = static_cast<size_t>(spline_length_ns_ / dt_ns_) + 1;

        if (verbose_)
        {
            std::cout<< "Initialized spline of order " << N_ 
            << " length " << nsec_to_sec(spline_length_ns_) 
            << " [s] with knot spacing " << nsec_to_sec(dt_ns_) 
            << " [s] representing quadrotor's angular velocities.\n";
        }
    }

    QuadAngVelSpline::~QuadAngVelSpline(){}

    int64_t QuadAngVelSpline::max_time_ns() const 
    {
        return start_t_ns_ + (knots_.size() - N_ + 1) * dt_ns_ - 1;
    }

    const double QuadAngVelSpline::max_time_s()
    {
        return nsec_to_sec(max_time_ns()) - 0.0001;
    }

    int64_t QuadAngVelSpline::min_time_ns() const 
    {
        return start_t_ns_; 
    }
    
    const double QuadAngVelSpline::min_time_s()
    { 
        return nsec_to_sec(min_time_ns()); 
    }

    const double QuadAngVelSpline::nsec_to_sec(const int64_t t)
    {
        return static_cast<double>(t*1e-9);
    }

    const int64_t QuadAngVelSpline::sec_to_nsec(const double t)
    {
        return static_cast<int64_t>(t*1e9);
    }

    void QuadAngVelSpline::set_knot(size_t i, Vec3& vec)
    {
        if(knots_.size() <= i)
        {
            return;
        }
        knots_[i] = vec;
    }

    void QuadAngVelSpline::get_knot(size_t i, Vec3& vec) const
    {
        if(knots_.size() <= i)
        {
            return;
        }
        vec = knots_[i];
    }

    void QuadAngVelSpline::set_knots(aligned_deque& vec, int64_t t_start) 
    {
        size_t i0 = 0;
        size_t n = vec.size();

        // debug
        // std::cout << "Im here 0\n";
        // end

        if (n > n_knots_)
        {
            i0 = n - n_knots_;
        }

        // debug
        // std::cout << "Im here 1\n";
        // end

        for (size_t i = i0; i < n; i++)
        {
            knots_.push_back(vec[i]);
            knots_times_.push_back(t_start + i * dt_ns_);
        }

        // debug
        // std::cout << "Im here 2\n";
        // end

        size_t curr_n_knots_ = knots_.size();
        size_t n_diff = 0;
        
        if (curr_n_knots_ > n_knots_)
        {
            n_diff = curr_n_knots_ - n_knots_;
        }

        // debug
        // std::cout << "Im here 3\n";
        // std::cout << "curr_n_knots_ = " << curr_n_knots_ << "\n";
        // std::cout << "n_knots_ = " << n_knots_ << "\n";
        // std::cout << "n_diff = " << n_diff << "\n";

        // std::cout << "knots_.size() = " << knots_.size() << "\n";
        // std::cout << "knots_times_.size() = " << knots_times_.size() << "\n";
        // end

        if (n_diff > 0)
        {
            for (size_t i = 0; i < n_diff; i++)
            {
                knots_.pop_front();
                knots_times_.pop_front();
            }
        }

        // debug
        // std::cout << "Im here 4\n";
        // end

        start_t_ns_ = knots_times_.front();

        // debug
        // std::cout << "Im here 5\n";
        // end

        if (verbose_)
        {
            std::cout <<  knots_.size() << " knots\n";
            std::cout << "start_t_ns_ = " << start_t_ns_ << "\n";
            std::cout << "knots_times: \n";
            for (const auto t : knots_times_)
            {
                std::cout << t << "\n";
            }
            std::cout << "\n";
            std::cout << "knots_values: \n";
            for (const auto v : knots_)
            {
                std::cout << v.x() << ", " << v.y() << ", " << v.z() << "\n";
            }
            std::cout << "\n";
        }

        // debug
        // std::cout << "Im here 6\n";
        // end

    }

    void QuadAngVelSpline::get_knots(aligned_deque& vec) const
    {
        vec = knots_;
    }

    void QuadAngVelSpline::get_knots_times(std::deque<int64_t>& vec) const
    {
        vec = knots_times_;
    }

    int64_t QuadAngVelSpline::get_time_interval_ns() const 
    { 
        return dt_ns_; 
    }

    const double QuadAngVelSpline::get_time_interval_s()
    { 
        return nsec_to_sec(dt_ns_); 
    }

    void QuadAngVelSpline::set_inertia(Mat3& J)
    {
        inertia_ = J;

        inertia_delta_(0) = inertia_(2,2) - inertia_(1,1);
        inertia_delta_(1) = inertia_(0,0) - inertia_(2,2);
        inertia_delta_(2) = inertia_(1,1) - inertia_(0,0);
    }

    const QuadAngVelSpline::Vec3 QuadAngVelSpline::evaluate(
        const int derivative, int64_t time_ns, JacobianStruct* J)
    {
        int64_t st_ns = (time_ns - start_t_ns_);
        
        DEBUG_CHECK(st_ns >= 0) << "st_ns < 0";

        int64_t s = st_ns / dt_ns_;
        double u = double(st_ns % dt_ns_) / double(dt_ns_);

        DEBUG_CHECK(s >= 0) "s >= 0";
        DEBUG_CHECK(size_t(s + N_) <= knots_.size()) "size_t(s + N_) > knots_.size()";
            
        VecN p;
        base_coeffs_with_time(u, p, derivative);

        VecN coeff = pow_inv_dt_[derivative] * (blending_matrix_ * p);

        Vec3 res;
        res.setZero();
        for (int i = 0; i < N_; i++) 
        {
            res += coeff[i] * knots_[s + i];
            
            if (J) 
            {
                J->d_val_d_knot[i] = coeff[i];
            }
        }

        if (J) 
        {
            J->start_idx = s;
        }

        return res;
    }

    const QuadAngVelSpline::Vec3 QuadAngVelSpline::angular_velocity_residual(
        int64_t time_ns, const Vec3 &measured_angvel, JacobianStruct *Jac)
    {
        return evaluate(0, time_ns, Jac) - measured_angvel;
    }

    const QuadAngVelSpline::Vec3 QuadAngVelSpline::angular_acceleration_residual(
        int64_t time_ns, const Vec3 &measured_angaccel, JacobianStruct *Jac)
    {
        return evaluate(1, time_ns, Jac) - measured_angaccel;
    }

    const QuadAngVelSpline::Vec3 QuadAngVelSpline::rot_dynamics_residual(
        int64_t time_ns, const Vec3 &torque_meas, JacobianStructRotDynamicsRes *Jac)
    {
        // residual
        JacobianStruct Jac_w;
        Vec3 w = evaluate(0, time_ns, &Jac_w);

        JacobianStruct Jac_wdot;
        Vec3 wdot = evaluate(1, time_ns, &Jac_wdot);

        Vec3 inertia_dot_wdot = inertia_ * wdot;
        Vec3 inertia_dot_w = inertia_ * w;
        Mat3 skew_w = skew(w);
        Vec3 w_cross_inertia_dot_w = skew_w * inertia_dot_w;
        Vec3 res = inertia_dot_wdot + w_cross_inertia_dot_w - torque_meas;

        if (Jac) 
        {
            int64_t st_ns = (time_ns - start_t_ns_);
            int64_t s = st_ns / dt_ns_;
            Jac->start_idx = s;

            for (size_t i = 0; i < N_; i++)
            {
                Mat3 d_res1_d_knotk;
                d_res1_d_knotk = inertia_ * Jac_wdot.d_val_d_knot[i];

                Mat3 pseudo_skew_w = pseudo_skew(w);
                pseudo_skew_w(0,1) *= inertia_delta_[0];
                pseudo_skew_w(0,2) *= inertia_delta_[0];
                pseudo_skew_w(1,0) *= inertia_delta_[1];
                pseudo_skew_w(1,2) *= inertia_delta_[1];
                pseudo_skew_w(2,0) *= inertia_delta_[2];
                pseudo_skew_w(2,1) *= inertia_delta_[2];
                Mat3 d_res2_d_knotk;
                d_res2_d_knotk = pseudo_skew_w * Jac_w.d_val_d_knot[i];

                Jac->d_res_d_knot[i] = d_res1_d_knotk + d_res2_d_knotk;
            }
        }

        return res;
    }

    const void QuadAngVelSpline::base_coeffs_with_time(
        const double t, const VecN& res_const, const int derivative)
    {
        VecN& res = const_cast<VecN&>(res_const);
        res.setZero();

        if (derivative < N_)
        {
            res[derivative] = base_coefficients_(derivative, derivative);

            double ti = t;
            for (int j = derivative + 1; j < N_; j++) 
            {
                res[j] = base_coefficients_(derivative, j) * ti;
                ti = ti * t;
            }
        }
    }

    void QuadAngVelSpline::apply_inc(const int i, const Vec3 &inc)
    {
        knots_[i] += inc;
    }

    QuadAngVelSpline::Mat3 skew(QuadAngVelSpline::Vec3& v)
    {
        QuadAngVelSpline::Mat3 M;
        M.setZero();
        M(0,1) = -v[2];
        M(0,2) = v[1];
        M(1,0) = v[2];
        M(1,2) = -v[0];
        M(2,0) = -v[1];
        M(2,1) = v[0];

        return M;
    }

    QuadAngVelSpline::Mat3 pseudo_skew(QuadAngVelSpline::Vec3& v)
    {
        QuadAngVelSpline::Mat3 M;
        M.setZero();
        M(0,1) = v[2];
        M(0,2) = v[1];
        M(1,0) = v[2];
        M(1,2) = v[0];
        M(2,0) = v[1];
        M(2,1) = v[0];

        return M;
    }

    const uint64_t binomial_coefficient(uint64_t n, uint64_t k)
    {
        if (k > n) 
        {
            return 0;
        }

        uint64_t r = 1;
        for (uint64_t d = 1; d <= k; ++d) 
        {
            r *= n--;
            r /= d;
        }
        return r;
    }

    const QuadAngVelSpline::MatN compute_base_coefficients(
        const int order, const int deg) 
    {
        QuadAngVelSpline::MatN base_coefficients;

        base_coefficients.setZero();
        base_coefficients.row(0).setOnes();
        
        int cnt = deg;
        for (int n = 1; n < order; n++) 
        {
            for (int i = deg - cnt; i < order; i++) 
            {
                base_coefficients(n, i) = (cnt - deg + i) * base_coefficients(n - 1, i);
            }
            cnt--;
        }
        return base_coefficients;
    }

    const QuadAngVelSpline::MatN compute_blending_matrix(
        const int order, const bool cumulative)
    {
        QuadAngVelSpline::MatN m;
        m.setZero();

        for (int i = 0; i < order; ++i) 
        {
            for (int j = 0; j < order; ++j) 
            {
                double sum = 0;
                for (int s = j; s < order; ++s) 
                {
                    sum += std::pow(-1.0, s - j) * binomial_coefficient(order, s - j) * std::pow(order - s - 1.0, order - 1.0 - i);
                }
                
                m(j, i) = binomial_coefficient(order - 1, order - 1 - i) * sum;
            }
        }

        if (cumulative) 
        {
            for (int i = 0; i < order; i++) 
            {
                for (int j = i + 1; j < order; j++) 
                {
                    m.row(i) += m.row(j);
                }
            }
        }

        uint64_t factorial = 1;
        for (int i = 2; i < order; ++i) 
        {
            factorial *= i;
        }

        m /= factorial;
        return m;
    }

}  // namespace svo