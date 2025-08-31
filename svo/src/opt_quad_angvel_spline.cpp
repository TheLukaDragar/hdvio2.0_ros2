#include <svo/opt_quad_angvel_spline.h>


namespace svo {

    OptimizeQuadAngVelSpline::OptimizeQuadAngVelSpline(){}

    OptimizeQuadAngVelSpline::OptimizeQuadAngVelSpline(
        const int64_t dt_ns, const int64_t spline_length_ns)
    {
        spline_ = QuadAngVelSpline(dt_ns, spline_length_ns);
    }

    OptimizeQuadAngVelSpline::~OptimizeQuadAngVelSpline(){}

    const int64_t OptimizeQuadAngVelSpline::get_dt_ns()
    {
        return spline_.get_dt_ns();
    }

    const double OptimizeQuadAngVelSpline::get_dt_s()
    {
        return nsec_to_sec(spline_.get_dt_ns());
    }

    void OptimizeQuadAngVelSpline::set_start_time_ns(const int64_t start_time_ns)
    {
        spline_.set_start_time_ns(start_time_ns);
    }

    void OptimizeQuadAngVelSpline::set_start_time_s(const double start_time_s)
    {
        const int64_t start_time_ns = sec_to_nsec(start_time_s);
        spline_.set_start_time_ns(start_time_ns);
    }

    const int64_t OptimizeQuadAngVelSpline::get_start_time_ns()
    {
        return spline_.get_start_time_ns();
    }

    const double OptimizeQuadAngVelSpline::get_start_time_s()
    {
        return nsec_to_sec(spline_.get_start_time_ns());
    }

    const double OptimizeQuadAngVelSpline::nsec_to_sec(const int64_t t)
    {
        return spline_.nsec_to_sec(t);
    }

    const int64_t OptimizeQuadAngVelSpline::sec_to_nsec(const double t)
    {
        return spline_.sec_to_nsec(t);
    }

    int64_t OptimizeQuadAngVelSpline::max_time_ns() const 
    {
        return spline_.max_time_ns();
    }

    const double OptimizeQuadAngVelSpline::max_time_s()
    {
        return spline_.max_time_s();
    }

    int64_t OptimizeQuadAngVelSpline::min_time_ns() const 
    {
        return spline_.min_time_ns(); 
    }
    
    const double OptimizeQuadAngVelSpline::min_time_s()
    { 
        return spline_.min_time_s(); 
    }

    void OptimizeQuadAngVelSpline::set_knots(aligned_deque& vec, int64_t t_start) 
    {
        spline_.set_knots(vec, t_start);
    }

    void OptimizeQuadAngVelSpline::sample(
        Vec3& val, const int derivative, int64_t time_ns)
    {
        val = spline_.evaluate(derivative, time_ns);
    }

    int OptimizeQuadAngVelSpline::get_order() const
    {
        return spline_.N_;
    }

    int OptimizeQuadAngVelSpline::get_degree() const
    {
        return spline_.DEG_;
    }

    const size_t OptimizeQuadAngVelSpline::get_num_knots() const
    {
        return spline_.get_num_knots();
    }

    void OptimizeQuadAngVelSpline::get_knot(size_t i, Vec3& knot) const
    {
        spline_.get_knot(i, knot);
    }

    void OptimizeQuadAngVelSpline::get_knots(aligned_deque& knots) const
    {
        spline_.get_knots(knots);
    }

    void OptimizeQuadAngVelSpline::get_knots_times(std::deque<int64_t>& vec) const
    {
        spline_.get_knots_times(vec);
    }

    size_t OptimizeQuadAngVelSpline::get_num_of_measurements(int mode) const
    {
        // compute current linearization point (error and jacobian)
        if (mode == 0)
        {
            return ang_vel_measurements_.size();
        }
        else if (mode == 1)
        {
            return ang_accel_measurements_.size();
        }
        else if (mode == 2)
        {
            return ang_vel_measurements_.size() + torque_measurements_.size();
        }
        else if (mode == 3)
        {
            return torque_measurements_.size();
        }
        else
        {
            DEBUG_CHECK(false) << "Unknown measurment mode!";
        }
    }

    void OptimizeQuadAngVelSpline::set_inertia(Mat3& J)
    {
        spline_.set_inertia(J);
    }

    void OptimizeQuadAngVelSpline::add_ang_vel_measurements(
        std::vector<double>& times,
        aligned_vector& ang_vel_measurements)
    {
        // push measurments in the buffer
        for(size_t i = 0; i < ang_vel_measurements_buffer_.size(); i++)
        {
            ang_vel_measurements_.emplace_back();
            ang_vel_measurements_.back().time = ang_vel_measurements_buffer_.at(i).time;
            ang_vel_measurements_.back().value = ang_vel_measurements_buffer_.at(i).value;
        }
        ang_vel_measurements_buffer_.clear();

        // push new measurments
        double t_last_meas = 0.0;
        if (ang_vel_measurements_.size() > 0)
        {
            t_last_meas = ang_vel_measurements_.back().time;
        }

        size_t n = times.size();
        for(size_t i = 0; i < n; i++)
        {
            double t = times.at(i);

            if (t > t_last_meas)
            {
                ang_vel_measurements_.emplace_back();
                ang_vel_measurements_.back().time = t;
                ang_vel_measurements_.back().value = ang_vel_measurements.at(i);
            }
        }

        // pop old measurments
        double t0 = min_time_s();
        size_t n_remove_front = 0;

        for (size_t i = 0; i < ang_vel_measurements_.size(); i++)
        {
            double t = ang_vel_measurements_.at(i).time;

            if (t < t0)
            {
                n_remove_front++;
            }
            else
            {
                break;
            }
        }    

        for (size_t i = 0; i < n_remove_front; i++)
        {
            // debug
            // std::cout << "Popped ang vel meas at time " << std::setprecision(15) 
            // << ang_vel_measurements_.front().time << "\n";
            // end

            ang_vel_measurements_.pop_front();
        }

        // the buffer contains measurements that can't be used right now, because to new,
        // but they can be used at the next iteration
        double t1 = max_time_s();
        size_t n_remove_back = 0;

        for (int i = ang_vel_measurements_.size()-1; i >=0; i--)
        {
            double t = ang_vel_measurements_.at(i).time;

            if (t > t1)
            {
                n_remove_back++;
            }
            else
            {
                break;
            }
        }

        for (size_t i = 0; i < n_remove_back; i++)
        {
            Measurement m;
            m.time = ang_vel_measurements_.back().time;
            m.value = ang_vel_measurements_.back().value;

            ang_vel_measurements_buffer_.push_front(m);

            ang_vel_measurements_.pop_back();
        }

        if (verbose_)
        {
            std::cout<< "Have " << ang_vel_measurements_.size() << " ang. velocities measurements\n";
            for (size_t i = 0; i < ang_vel_measurements_.size(); i++)
            {
              std::cout << "ang_vel_measurements_[" << i << "], time = " << std::setprecision(15) 
              << ang_vel_measurements_[i].time
              << ", val = " << ang_vel_measurements_[i].value(0) << ", " 
              << ang_vel_measurements_[i].value(1) << ", " 
              << ang_vel_measurements_[i].value(2) << "\n";
            }
            std::cout << "\n";

            // std::cout<< "Have " << ang_vel_measurements_buffer_.size() << " ang. velocities measurements in buffer\n";
            // for (size_t i = 0; i < ang_vel_measurements_buffer_.size(); i++)
            // {
            //   std::cout << "ang_vel_measurements_buffer_[" << i << "], time = " << std::setprecision(15) 
            //   << ang_vel_measurements_buffer_[i].time
            //   << ", val = " << ang_vel_measurements_buffer_[i].value(0) << ", " 
            //   << ang_vel_measurements_buffer_[i].value(1) << ", " 
            //   << ang_vel_measurements_buffer_[i].value(2) << "\n";
            // }
            // std::cout << "\n";
        }
    }

    void OptimizeQuadAngVelSpline::add_ang_accel_measurements(
        std::vector<double>& times,
        aligned_vector& ang_accel_measurements)
    {
        // push measurments in the buffer
        for(size_t i = 0; i < ang_accel_measurements_buffer_.size(); i++)
        {
            ang_accel_measurements_.emplace_back();
            ang_accel_measurements_.back().time = ang_accel_measurements_buffer_.at(i).time;
            ang_accel_measurements_.back().value = ang_accel_measurements_buffer_.at(i).value;
        }
        ang_accel_measurements_buffer_.clear();

        // push new measurments
        double t_last_meas = 0.0;
        if (ang_accel_measurements_.size() > 0)
        {
            t_last_meas = ang_accel_measurements_.back().time;
        }

        size_t n = times.size();
        for(size_t i = 0; i < n; i++)
        {
            double t = times.at(i);

            if (t > t_last_meas)
            {
                ang_accel_measurements_.emplace_back();
                ang_accel_measurements_.back().time = t;
                ang_accel_measurements_.back().value = ang_accel_measurements.at(i);
            }
        }

        // pop old measurments
        double t0 = min_time_s();
        size_t n_remove_front = 0;

        for (size_t i = 0; i < ang_accel_measurements_.size(); i++)
        {
            double t = ang_accel_measurements_.at(i).time;

            if (t < t0)
            {
                n_remove_front++;
            }
            else
            {
                break;
            }
        }    

        for (size_t i = 0; i < n_remove_front; i++)
        {
            ang_accel_measurements_.pop_front();
        }

        // the buffer contains measurements that can't be used right now, because to new,
        // but they can be used at the next iteration
        double t1 = max_time_s();
        size_t n_remove_back = 0;

        for (int i = ang_accel_measurements_.size()-1; i >=0; i--)
        {
            double t = ang_accel_measurements_.at(i).time;

            if (t > t1)
            {
                n_remove_back++;
            }
            else
            {
                break;
            }
        }

        for (size_t i = 0; i < n_remove_back; i++)
        {
            Measurement m;
            m.time = ang_accel_measurements_.back().time;
            m.value = ang_accel_measurements_.back().value;

            ang_accel_measurements_buffer_.push_front(m);

            ang_accel_measurements_.pop_back();
        }

        if (verbose_)
        {
            std::cout<< "Added " << ang_accel_measurements_.size() << " ang. acceleration measurements\n";
        }
    }

    void OptimizeQuadAngVelSpline::add_rot_dynamics_measurements(
        std::vector<double>& times,
        aligned_vector& rot_dynamics_measurements)
    {
        // push measurments in the buffer
        for(size_t i = 0; i < torque_measurements_buffer_.size(); i++)
        {
            torque_measurements_.emplace_back();
            torque_measurements_.back().time = torque_measurements_buffer_.at(i).time;
            torque_measurements_.back().value = torque_measurements_buffer_.at(i).value;
        }
        torque_measurements_buffer_.clear();

        // push new measurements
        double t_last_meas = 0.0;
        if (torque_measurements_.size() > 0)
        {
            t_last_meas = torque_measurements_.back().time;
        }

        size_t n = times.size();
        for(size_t i = 0; i < n; i++)
        {
            double t = times.at(i);

            if (t > t_last_meas)
            {
                torque_measurements_.emplace_back();
                torque_measurements_.back().time = t;
                torque_measurements_.back().value = rot_dynamics_measurements.at(i);
            }
        }

        // pop old measurments
        double t0 = min_time_s();
        size_t n_remove_front = 0;

        for (size_t i = 0; i < torque_measurements_.size(); i++)
        {
            double t = torque_measurements_.at(i).time;

            if (t < t0)
            {
                n_remove_front++;
            }
            else
            {
                break;
            }
        }    

        for (size_t i = 0; i < n_remove_front; i++)
        {
            torque_measurements_.pop_front();
        }

        // the buffer contains measurements that can't be used right now, because to new,
        // but they can be used at the next iteration
        double t1 = max_time_s();
        size_t n_remove_back = 0;

        for (int i = torque_measurements_.size()-1; i >=0; i--)
        {
            double t = torque_measurements_.at(i).time;

            if (t > t1)
            {
                n_remove_back++;
            }
            else
            {
                break;
            }
        }

        for (size_t i = 0; i < n_remove_back; i++)
        {
            Measurement m;
            m.time = torque_measurements_.back().time;
            m.value = torque_measurements_.back().value;

            torque_measurements_buffer_.push_front(m);

            torque_measurements_.pop_back();
        }

        if (verbose_)
        {
            std::cout<< "Have " << torque_measurements_.size() << " torque measurements\n";
            for (size_t i = 0; i < torque_measurements_.size(); i++)
            {
                std::cout << "torque_measurements_[" << i << "], time = " << std::setprecision(15) 
                << torque_measurements_[i].time
                << ", val = " << torque_measurements_[i].value(0) << ", " 
                << torque_measurements_[i].value(1) << ", " 
                << torque_measurements_[i].value(2) << "\n";
            }
            std::cout << "\n";
        }
    }

    double OptimizeQuadAngVelSpline::compute_ang_vel_errors(
        bool compute_jacobian)
    {
        double error = 0.0;

        for (const Measurement& m : ang_vel_measurements_)
        {
            int64_t start_idx;
            QuadAngVelSpline::JacobianStruct Jac;

            const int64_t time_ns = sec_to_nsec(m.time);
            assert(time_ns >= spline_.min_time_ns());
            
            // compute residual
            Vec3 residual = Vec3::Zero();
            if (compute_jacobian)
            {
                residual = spline_.angular_velocity_residual(time_ns, m.value, &Jac);
            }
            else
            {
                residual = spline_.angular_velocity_residual(time_ns, m.value);
            }
            
            error  += ang_vel_var_inv_ * residual.squaredNorm();

            if (compute_jacobian)
            {
                // compute hessian entries
                start_idx = Jac.start_idx;

                const int n = spline_.N_;
                for (size_t i = 0; i < n; i++)
                {
                    size_t start_i = (start_idx + i) * 3;

                    /// @todo add assertion
                    /// start_i < opt_problem_size_ - 3

                    for (size_t j = 0; j <= i; j++)
                    {
                        size_t start_j = (start_idx + j) * 3;

                        /// @todo add assertion
                        /// start_j < opt_problem_size_ - 3

                        Key id;
                        id[0] = start_i;
                        id[1] = start_j;
                        id[2] = 3;
                        id[3] = 3;
                        
                        Mat3 data = ang_vel_var_inv_ * Jac.d_val_d_knot[i] * Jac.d_val_d_knot[j] * Mat3::Identity();

                        auto it = hash_map_.find(id);
                        if (it == hash_map_.end()) 
                        {
                            hash_map_.emplace(id, data);
                        } 
                        else 
                        {
                            it->second += data;
                        }   
                    }

                    b_.segment<3>(start_i) += ang_vel_var_inv_ * Jac.d_val_d_knot[i] * residual;
                }
            }
        }

        return error;
    }

    double OptimizeQuadAngVelSpline::compute_ang_accel_errors(
        bool compute_jacobian)
    {
        double error = 0.0;

        for (const Measurement& m : ang_accel_measurements_)
        {
            int64_t start_idx;
            QuadAngVelSpline::JacobianStruct Jac;

            const int64_t time_ns = sec_to_nsec(m.time);
            assert(time_ns >= spline_.min_time_ns());
            
            // compute residual
            Vec3 residual = Vec3::Zero();
            if (compute_jacobian)
            {
                residual = spline_.angular_acceleration_residual(time_ns, m.value, &Jac);
            }
            else
            {
                residual = spline_.angular_acceleration_residual(time_ns, m.value);
            }
            
            error  += ang_accel_std_ * residual.squaredNorm();

            if (compute_jacobian)
            {
                // compute hessian entries
                start_idx = Jac.start_idx;

                const int n = spline_.N_;
                for (size_t i = 0; i < n; i++)
                {
                    size_t start_i = (start_idx + i) * 3;

                    /// @todo add assertion
                    /// start_i < opt_problem_size_ - 3

                    for (size_t j = 0; j <= i; j++)
                    {
                        size_t start_j = (start_idx + j) * 3;

                        /// @todo add assertion
                        /// start_j < opt_problem_size_ - 3

                        Key id;
                        id[0] = start_i;
                        id[1] = start_j;
                        id[2] = 3;
                        id[3] = 3;
                        Mat3 data = ang_accel_std_ * Jac.d_val_d_knot[i] * Jac.d_val_d_knot[j] * Mat3::Identity();

                        auto it = hash_map_.find(id);
                        if (it == hash_map_.end()) 
                        {
                            hash_map_.emplace(id, data);
                        } 
                        else 
                        {
                            it->second += data;
                        }   
                    }

                    b_.segment<3>(start_i) += ang_accel_std_ * Jac.d_val_d_knot[i] * residual;
                }
            }
        }

        return error;
    }

    double OptimizeQuadAngVelSpline::compute_rot_dynamics_errors(
        bool compute_jacobian)
    {
        double error = 0.0;

        for (const Measurement& m : torque_measurements_)
        {
            int64_t start_idx;
            QuadAngVelSpline::JacobianStructRotDynamicsRes Jac;

            const int64_t time_ns = sec_to_nsec(m.time);
            assert(time_ns >= spline_.min_time_ns());
            
            // compute residual
            Vec3 residual = Vec3::Zero();
            if (compute_jacobian)
            {
                residual = spline_.rot_dynamics_residual(time_ns, m.value, &Jac);
            }
            else
            {
                residual = spline_.rot_dynamics_residual(time_ns, m.value);
            }
            
            error  += torque_var_inv_ * residual.squaredNorm();

            if (compute_jacobian)
            {
                // compute hessian entries
                start_idx = Jac.start_idx;

                const int n = spline_.N_;
                for (size_t i = 0; i < n; i++)
                {
                    size_t start_i = (start_idx + i) * 3;

                    /// @todo add assertion
                    /// start_i < opt_problem_size_ - 3

                    for (size_t j = 0; j <= i; j++)
                    {
                        size_t start_j = (start_idx + j) * 3;

                        /// @todo add assertion
                        /// start_j < opt_problem_size_ - 3

                        Key id;
                        id[0] = start_i;
                        id[1] = start_j;
                        id[2] = 3;
                        id[3] = 3;
                        Mat3 data = torque_var_inv_ * Jac.d_res_d_knot[i] * Jac.d_res_d_knot[j];

                        auto it = hash_map_.find(id);
                        if (it == hash_map_.end()) 
                        {
                            hash_map_.emplace(id, data);
                        } 
                        else 
                        {
                            it->second += data;
                        }   
                    }

                    b_.segment<3>(start_i) += torque_var_inv_ * Jac.d_res_d_knot[i] * residual;
                }
            }
        }

        return error;
    }

    bool OptimizeQuadAngVelSpline::optimize(
        double stop_thresh, double& error, int mode, bool print_info)
    {
        // reset 
        bool converged = false;
        bool step = false;
        int max_iter = 3;
        opt_problem_size_ = spline_.get_num_knots() * 3;
        double error_pre = 0.0;
        hash_map_.clear();
        b_.setZero(opt_problem_size_);

        // compute current linearization point (error and jacobian)
        if (mode == 0)
        {
            error_pre = compute_ang_vel_errors(true);
        }
        else if (mode == 1)
        {
            error_pre = compute_ang_accel_errors(true);
        }
        else if (mode == 2)
        {
            error_pre += compute_ang_vel_errors(true);
            error_pre += compute_rot_dynamics_errors(true);
        }
        else if (mode == 3)
        {
            error_pre = compute_rot_dynamics_errors(true);
        }
        else
        {
            DEBUG_CHECK(false) << "Unknown optimization mode!";
        }

        if (print_info)
        {
            std::cout << "Error pre optimization: " << error_pre << "\n\n";
        }

        // setup solver
        // fill in hessian matrix
        std::vector<Eigen::Triplet<double>> triplets;
        triplets.reserve(hash_map_.size() * 3 + b_.rows());
        for (const auto& kv : hash_map_) 
        {
            for (int i = 0; i < kv.second.rows(); i++) 
            {
                for (int j = 0; j < kv.second.cols(); j++) 
                {
                    triplets.emplace_back(
                        kv.first[0] + i, kv.first[1] + j, kv.second(i, j));
                }
            }
        }
        for (int i = 0; i < b_.rows(); i++) 
        {
            triplets.emplace_back(i, i, std::numeric_limits<double>::min());
        }

        smm_ = Eigen::SparseMatrix<double>(b_.rows(), b_.rows());
        smm_.setFromTriplets(triplets.begin(), triplets.end());

        /// @todo can I avoid a dynamic vector here?
        Eigen::VectorXd Hdiag = smm_.diagonal();

        // optimization loop
        while (!step && max_iter > 0 && !converged)
        {
            /// @todo can I avoid a dynamic vector here?
            Eigen::VectorXd Hdiag_lambda = Hdiag * lambda_;
            for (int i = 0; i < Hdiag_lambda.size(); i++)
            {
                Hdiag_lambda[i] = std::max(Hdiag_lambda[i], min_lambda_);
            }

            // solve
            Eigen::SparseMatrix<double> sm = smm_;

            sm.diagonal() += Hdiag_lambda;

            Eigen::VectorXd opt_res;

            auto t1 = std::chrono::high_resolution_clock::now();

            /// @note ConjugateGradient might be not necessary for our small size problem.
            // Solve with ConjugateGradient.
            // According to Eigen doc: https://eigen.tuxfamily.org/dox/classEigen_1_1ConjugateGradient.html
            // the Lower || Upper representation is more efficient.
            // However, the current Hessian (= smm_) is a lower triangular matrix
            // ==> we need to duplicate the lower part entries to the upper part.
            if (iterative_solver_)
            {
                Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower> cg;
                // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> cg;

                double tolerance = 1e-4;
                cg.setTolerance(tolerance);
                cg.compute(sm);
                opt_res = cg.solve(b_);
            }
            else
            {
                // Use Cholesky factorizations without square root
                Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> chol(sm);
                opt_res = chol.solve(b_);

                /// @todo compile with SuiteSparse to be able to use it
                // Use the Cholmod. This is the best option for dense enough problems
                // Not sure this is our case.
                // Eigen::CholmodSupernodalLLT<Eigen::SparseMatrix<double>> chol(smm_);
                // opt_res = chol.solve(b_);
            }

            auto t2 = std::chrono::high_resolution_clock::now();
            auto t_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);

            if (print_info)
            {
                std::cout << "Solving linear system took: " << t_elapsed.count()*1e-3 << " [ms]. \n";
            }

            Eigen::VectorXd inc_full;
            inc_full = -1.0 * opt_res;

            // check if converged
            double max_inc = inc_full.array().abs().maxCoeff();
            if (max_inc < stop_thresh)
            {
                converged = true;
            }

            // backup current linearization point
            QuadAngVelSpline spline_backup = spline_;

            // apply result
            apply_inc(inc_full);

            // recompute error
            double error_post = 0.0;
            if (mode == 0)
            {
                error_post = compute_ang_vel_errors(false);
            }
            else if (mode == 1)
            {
                error_post = compute_ang_accel_errors(false);
            }
            else if (mode == 2)
            {
                error_post += compute_ang_vel_errors(false);
                error_post += compute_rot_dynamics_errors(false);
            }
            else if (mode == 3)
            {
                error_post = compute_rot_dynamics_errors(false);
            }

            if (print_info)
            {
                std::cout << "error post optimization: " << error_post << "\n\n";
            }

            double f_diff = (error_pre - error_post);
            double l_diff = 0.5 * inc_full.dot(inc_full * lambda_ - b_);

            double step_quality = f_diff / l_diff;
            
            if (step_quality <= 0) 
            {
                if (print_info)
                {
                    std::cout << "\t[REJECTED] lambda:" << lambda_ 
                    << ", step_quality: " << step_quality << ", max_inc: " << max_inc << ". Error: " << error_post <<"\n";
                }
                
                lambda_ = std::min(max_lambda_, lambda_vee_ * lambda_);
                lambda_vee_ *= 2;

                // reject increment
                spline_ = spline_backup;
            }
            else
            {
                if (print_info)
                {
                    std::cout << "\t[ACCEPTED] lambda:" << lambda_ << ", step_quality: " << step_quality << ", max_inc: " << max_inc << ". Error: " << error_post <<"\n";
                }

                lambda_ = std::max(min_lambda_, lambda_ * std::max(1.0 / 3, 1 - std::pow(2 * step_quality - 1, 3.0)));
                lambda_vee_ = 2;

                error = error_post;

                step = true;
            }

            max_iter--;
        }

        if (converged && print_info)
        {
            std::cout << "\t[CONVERGED]\n";
        }

        return converged;
    }

    void OptimizeQuadAngVelSpline::apply_inc(const Eigen::VectorXd& inc_full)
    {
        size_t num_knots = spline_.get_num_knots();

        for (size_t i = 0; i < num_knots; i++) 
        {
            Vec3 inc = inc_full.segment<3>(i*3);
            spline_.apply_inc(i, inc);
        }
    }

} // namespace svo