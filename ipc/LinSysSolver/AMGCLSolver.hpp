//
//  AMGCLSolver.hpp
//  IPC
//
//  Created by Minchen Li on 11/06/19.
//

#ifndef AMGCLSolver_hpp
#define AMGCLSolver_hpp

#include "ipc/LinSysSolver/LinSysSolver.hpp"

#include <amgcl/backend/builtin.hpp>
#include <amgcl/make_solver.hpp>
#include <amgcl/solver/cg.hpp>
#include <amgcl/solver/lgmres.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/coarsening/smoothed_aggregation.hpp>
#include <amgcl/coarsening/plain_aggregates.hpp>
#include <amgcl/coarsening/aggregation.hpp>
#include <amgcl/coarsening/ruge_stuben.hpp>
#include <amgcl/relaxation/spai0.hpp>
#include <amgcl/relaxation/gauss_seidel.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/adapter/block_matrix.hpp>
#include <amgcl/solver/bicgstab.hpp>
#include <amgcl/solver/gmres.hpp>
#include <amgcl/solver/runtime.hpp>
#include <amgcl/profiler.hpp>
#include <amgcl/io/mm.hpp>
#include <amgcl/relaxation/chebyshev.hpp>
#include <amgcl/coarsening/runtime.hpp>
#include <amgcl/relaxation/runtime.hpp>
#include <amgcl/preconditioner/runtime.hpp>
#include <amgcl/value_type/static_matrix.hpp>
#include <amgcl/adapter/reorder.hpp>
#include <amgcl/adapter/eigen.hpp>
#include <amgcl/profiler.hpp>

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Eigen>

#include <vector>
#include <set>

// #define USE_BW_BACKEND // use blockwise backend, must undef USE_BW_AGGREGATION
// #define USE_BW_AGGREGATION // use scalar backend but blockwise aggregation, must undef USE_BW_BACKEND
// if neither of above is defined, will use scalar backend and aggregation

// #define USE_AMG_SOLVER // use a single V-cycle to approximately solve the linear system
// if not defined then will use V-cycle preconditioned CG to solve the linear system

namespace IPC {

template <typename vectorTypeI, typename vectorTypeS>
class AMGCLSolver : public LinSysSolver<vectorTypeI, vectorTypeS> {
    typedef LinSysSolver<vectorTypeI, vectorTypeS> Base;

#ifdef USE_BW_BACKEND
    typedef amgcl::static_matrix<double, DIM, DIM> value_type;
    typedef amgcl::static_matrix<double, DIM, 1> rhs_type;
    typedef amgcl::backend::builtin<value_type> Backend;
    // using Solver = amgcl::make_solver<
    //     // Use AMG as preconditioner:
    //     amgcl::amg<
    //         BBackend,
    //         amgcl::coarsening::smoothed_aggregation,
    //         amgcl::relaxation::gauss_seidel>,
    //     // And BiCGStab as iterative solver:
    //     amgcl::solver::lgmres<BBackend>>;
#else
    typedef amgcl::backend::builtin<double> Backend;
    // // Use AMG as preconditioner:
    // typedef amgcl::make_solver<
    //     // Use AMG as preconditioner:
    //     amgcl::amg<
    //         Backend,
    //         amgcl::coarsening::smoothed_aggregation,
    //         amgcl::relaxation::gauss_seidel>,
    //     // And CG as iterative solver:
    //     amgcl::solver::lgmres<Backend>>
    //     Solver;
#endif
    using Solver = amgcl::make_solver<
        amgcl::runtime::preconditioner<Backend>,
        amgcl::runtime::solver::wrapper<Backend>>;

protected:
    Solver* solver;
    boost::property_tree::ptree params;
    std::vector<int> _ia, _ja;
    std::vector<double> _a;

public:
    AMGCLSolver(void);
    ~AMGCLSolver(void);

    LinSysSolverType type() const override { return LinSysSolverType::AMGCL; }

    void set_pattern(const std::vector<std::set<int>>& vNeighbor,
        const std::set<int>& fixedVert);
    void load(const char* filePath, Eigen::VectorXd& rhs);

    void load_AMGCL(const char* filePath, Eigen::VectorXd& rhs);
    void write_AMGCL(const char* filePath, const Eigen::VectorXd& rhs) const;

    void copyOffDiag_IJ(void);
    void copyOffDiag_a(void);

    void analyze_pattern(void);

    bool factorize(void);

    void solve(Eigen::VectorXd& rhs, Eigen::VectorXd& result);
};

} // namespace IPC

#endif /* AMGCLSolver_hpp */
