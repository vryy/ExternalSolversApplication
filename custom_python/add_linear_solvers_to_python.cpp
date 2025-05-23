//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Riccardo Rossi
//

// System includes

// External includes
#include <boost/python.hpp>


// Project includes
#include "includes/define.h"
#include "includes/kratos_parameters.h"
#include "includes/model_part.h"
#include "python/add_equation_systems_to_python.h"
#include "spaces/ublas_space.h"
//#include "spaces/parallel_ublas_space.h"

#include "linear_solvers/direct_solver.h"
#include "linear_solvers/iterative_solver.h"
#include "linear_solvers/skyline_lu_factorization_solver.h"
#include "external_includes/gmres_solver.h"

#ifdef ENABLE_SUPERLU
  #include "external_includes/superlu_solver.h"
  #include "external_includes/superlu_iterative_solver.h"
#endif

#ifndef EXCLUDE_ITSOL
  #include "external_includes/itsol_arms_solver.h"
#endif

#ifdef INCLUDE_PASTIX
  #include "external_includes/pastix_solver.h"
#endif

#ifndef EXCLUDE_AMGCL
  #include "external_includes/amgcl_solver.h"
  #include "external_includes/amgcl_ns_solver.h"
#endif

namespace Kratos
{

namespace Python
{

void  AddLinearSolversToPython()
{
    typedef UblasSpace<double, CompressedMatrix, Vector> SpaceType;
    //      typedef ParallelUblasSpace<double, CompressedMatrix, Vector> ParallelSpaceType;
    typedef UblasSpace<double, Matrix, Vector> LocalSpaceType;
    // typedef UblasSpace<double, Matrix, Vector> ParallelLocalSpaceType;
    typedef LinearSolver<SpaceType, LocalSpaceType, ModelPart> LinearSolverType;
    //typedef LinearSolver<ParallelSpaceType,  ParallelLocalSpaceType> ParallelLinearSolverType;
    typedef DirectSolver<SpaceType, LocalSpaceType, ModelPart> DirectSolverType;
    //typedef Reorderer<ParallelSpaceType,  ParallelLocalSpaceType > ParallelReordererType;
    //typedef DirectSolver<ParallelSpaceType,  ParallelLocalSpaceType, ParallelReordererType > ParallelDirectSolverType;
    typedef IterativeSolver<SpaceType, LocalSpaceType, ModelPart> IterativeSolverType;
    typedef GMRESSolver<SpaceType, LocalSpaceType, ModelPart> GMRESSolverType;
    typedef Preconditioner<SpaceType, LocalSpaceType, ModelPart> PreconditionerType;

    using namespace boost::python;

    //***************************************************************************
    //linear solvers
    //***************************************************************************

#ifdef ENABLE_SUPERLU
    typedef SuperLUSolver<SpaceType, LocalSpaceType, ModelPart> SuperLUSolverType;
    typedef SuperLUIterativeSolver<SpaceType, LocalSpaceType, ModelPart> SuperLUIterativeSolverType;
    class_<SuperLUSolverType, bases<DirectSolverType>, boost::noncopyable >
    ( "SuperLUSolver", init<>() )
    .def("AdditionalPhysicalDataIsNeeded", &SuperLUSolverType::AdditionalPhysicalDataIsNeeded)
    .def("ProvideAdditionalData", &SuperLUSolverType::ProvideAdditionalData)
    ;

    class_<SuperLUIterativeSolverType, bases<LinearSolverType>, boost::noncopyable >
    ( "SuperLUIterativeSolver",init<>() )
    .def(init<double,int,int,double,double,double>())
    ;
#endif

#ifndef EXCLUDE_ITSOL
    typedef ITSOL_ARMS_Solver<SpaceType, LocalSpaceType, ModelPart> ITSOL_ARMS_SolverType;
    class_<ITSOL_ARMS_SolverType, bases<LinearSolverType>, boost::noncopyable >
    ( "ITSOL_ARMS_Solver",init<>() )
    .def(init<double,int,int>())
    ;
#endif

#ifdef INCLUDE_PASTIX
    typedef PastixSolver<SpaceType, LocalSpaceType, ModelPart> PastixSolverType;
    class_<PastixSolverType, bases<LinearSolverType>, boost::noncopyable >
    ( "PastixSolver",init<int,bool>() )
    .def(init<double,int,int,int,bool>())
    ;
#endif

#ifndef EXCLUDE_AMGCL
     enum_<AMGCLSmoother>("AMGCLSmoother")
    .value("SPAI0", SPAI0)
    .value("ILU0", ILU0)
    .value("DAMPED_JACOBI",DAMPED_JACOBI)
    .value("GAUSS_SEIDEL",GAUSS_SEIDEL)
    .value("CHEBYSHEV",CHEBYSHEV)
    ;

    enum_<AMGCLIterativeSolverType>("AMGCLIterativeSolverType")
    .value("GMRES", GMRES)
    .value("BICGSTAB", BICGSTAB)
    .value("CG",CG)
    .value("BICGSTAB_WITH_GMRES_FALLBACK",BICGSTAB_WITH_GMRES_FALLBACK)
    .value("BICGSTAB2",BICGSTAB2)
    ;

    enum_<AMGCLCoarseningType>("AMGCLCoarseningType")
    .value("RUGE_STUBEN", RUGE_STUBEN)
    .value("AGGREGATION", AGGREGATION)
    .value("SA",SA)
    .value("SA_EMIN",SA_EMIN)
    ;

    typedef AMGCLSolver<SpaceType, LocalSpaceType, ModelPart> AMGCLSolverType;
    class_<AMGCLSolverType, bases<LinearSolverType>, boost::noncopyable >
    ( "AMGCLSolver", init<AMGCLSmoother,AMGCLIterativeSolverType,double,int,int,int>() )
    .def(init<AMGCLSmoother, AMGCLIterativeSolverType, AMGCLCoarseningType, double, int, int, int, bool>())
    .def(init<Parameters&>())
    .def("AdditionalPhysicalDataIsNeeded", &AMGCLSolverType::AdditionalPhysicalDataIsNeeded)
    .def("ProvideAdditionalData", &AMGCLSolverType::ProvideAdditionalData)
    ;

   // typedef AMGCL_NS_Solver<SpaceType, LocalSpaceType, ModelPart> AMGCL_NS_SolverType;
   // class_<AMGCL_NS_SolverType, bases<LinearSolverType>, boost::noncopyable >
   // ( "AMGCL_NS_Solver", init<AMGCLSmoother,AMGCLIterativeSolverType,AMGCLCoarseningType ,double,int,int,int>())
   // ;

#endif

    class_<GMRESSolverType, bases<IterativeSolverType>, boost::noncopyable >
    ( "GMRESSolver")
    .def(init<double>())
    .def(init<double, unsigned int>())
    .def(init<double, unsigned int, PreconditionerType::Pointer>())
    .def(self_ns::str(self))
    ;

}

}  // namespace Python.

} // Namespace Kratos
