#if !defined(KRATOS_PastixSolver )
#define  KRATOS_PastixSolver

// External includes
#include "boost/smart_ptr.hpp"

#include "includes/ublas_interface.h"
// #include "external_includes/superlu/superlu.hpp"

//#include "solveARMS.h"

// Project includes
#include "includes/define.h"
#include "linear_solvers/direct_solver.h"

//#include <complex.h>
/* to access functions from the libpastix, respect this order */
//#include "custom_external_libraries/solvePASTIX.c"

namespace ublas = boost::numeric::ublas;

namespace Kratos
{

extern "C"
{
    int solvePASTIX(int verbosity,int mat_size, int nnz, double* AA, size_t* IA, size_t* JA, double* x, double* b, int m_gmres,
                    double tol, int incomplete, int ilu_level_of_fill, int ndof, int symmetric);
}


template< class TSparseSpaceType, class TDenseSpaceType,
          class TModelPartType,
          class TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType> >
class PastixSolver : public DirectSolver< TSparseSpaceType,
    TDenseSpaceType, TModelPartType, TReordererType>
{
public:
    /**
     * Counted pointer of PastixSolver
     */
    KRATOS_CLASS_POINTER_DEFINITION( PastixSolver );

    typedef DirectSolver<TSparseSpaceType, TDenseSpaceType, TModelPartType, TReordererType> BaseType;

    typedef typename TSparseSpaceType::MatrixType SparseMatrixType;

    typedef typename TSparseSpaceType::VectorType VectorType;

    typedef typename TDenseSpaceType::MatrixType DenseMatrixType;

    typedef typename BaseType::ModelPartType ModelPartType;

    /**
     * Default constructor - uses ILU+GMRES
     * @param NewMaxTolerance tolerance that will be achieved by the iterative solver
     * @param NewMaxIterationsNumber this number represents both the number of iterations AND the size of the krylov space
     * @param level of fill that will be used in the ILU
     * @param verbosity, a number from 0 (no output) to 2 (maximal output)
     * @param is_symmetric, set to True to solve assuming the matrix is symmetric
     */
    PastixSolver(double NewMaxTolerance,
                 int NewMaxIterationsNumber,
                 int level_of_fill,
                 int verbosity,
                 bool is_symmetric) : BaseType()
    {
        std::cout << "setting up pastix for iterative solve " << std::endl;
        mTol = NewMaxTolerance;
        mmax_it = NewMaxIterationsNumber;
        mlevel_of_fill = level_of_fill;
        mincomplete = 1;
        mverbosity=verbosity;

        if(is_symmetric == false)
            msymmetric = 0;
        else
            msymmetric = 1;

        mndof = 1;
    }

    /**
     * Direct Solver
     * @param verbosity, a number from 0 (no output) to 2 (maximal output)
     * @param is_symmetric, set to True to solve assuming the matrix is symmetric
     */
    PastixSolver(int verbosity, bool is_symmetric) : BaseType()
    {
        std::cout << "setting up pastix for direct solve " <<std::endl;
        mTol = -1;
        mmax_it = -1;
        mlevel_of_fill = -1;
        mincomplete = 0;
        mverbosity=verbosity;

        mndof = 1;
        if(is_symmetric == false)
            msymmetric = 0;
        else
            msymmetric = 1; }

    /**
     * Destructor
     */
    ~PastixSolver() override {};

    /**
     * Normal solve method.
     * Solves the linear system Ax=b and puts the result on SystemVector& rX.
     * rX is also th initial guess for iterative methods.
     * @param rA. System matrix
     * @param rX. Solution vector.
     * @param rB. Right hand side vector.
     */
    bool Solve(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override
    {
            //KRATOS_WATCH(__LINE__);

        int state = solvePASTIX(mverbosity, rA.size1(), rA.value_data().size(), rA.value_data().begin(), &(rA.index1_data()[0]), &(rA.index2_data()[0]), &rX[0], &rB[0]
        ,mmax_it,mTol,mincomplete,mlevel_of_fill,mndof,msymmetric);

        return state;
    }

    /**
     * Multi solve method for solving a set of linear systems with same coefficient matrix.
     * Solves the linear system Ax=b and puts the result on SystemVector& rX.
     * rX is also th initial guess for iterative methods.
     * @param rA. System matrix
     * @param rX. Solution vector.
     * @param rB. Right hand side vector.
     */
    bool Solve(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB) override
    {

        bool is_solved = true;


        return is_solved;
    }

    /**
     * Print information about this object.
     */
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "Pastix solver finished.";
    }

    /**
     * Print object's data.
     */
    void PrintData(std::ostream& rOStream) const override
    {
    }

    /** Some solvers may require a minimum degree of knowledge of the structure of the matrix. To make an example
     * when solving a mixed u-p problem, it is important to identify the row associated to v and p.
     * another example is the automatic prescription of rotation null-space for smoothed-aggregation solvers
     * which require knowledge on the spatial position of the nodes associated to a given dof.
     * This function tells if the solver requires such data
     */
    bool AdditionalPhysicalDataIsNeeded() override
    {
        return true;
    }

    /** Some solvers may require a minimum degree of knowledge of the structure of the matrix. To make an example
     * when solving a mixed u-p problem, it is important to identify the row associated to v and p.
     * another example is the automatic prescription of rotation null-space for smoothed-aggregation solvers
     * which require knowledge on the spatial position of the nodes associated to a given dof.
     * This function is the place to eventually provide such data
     */
    void ProvideAdditionalData (
        SparseMatrixType& rA,
        VectorType& rX,
        VectorType& rB,
        typename ModelPartType::DofsArrayType& rdof_set,
        ModelPartType& r_model_part
    ) override
    {
        int old_ndof = -1;
        unsigned int old_node_id = rdof_set.begin()->Id();
        int ndof=0;

        for (auto it = rdof_set.begin(); it!=rdof_set.end(); it++)
        {
            if(it->EquationId() < rA.size1() )
            {
                unsigned int id = it->Id();
                if(id != old_node_id)
                {
                    old_node_id = id;
                    if(old_ndof == -1) old_ndof = ndof;
                    else if(old_ndof != ndof) //if it is different than the block size is 1
                    {
                        old_ndof = -1;
                        break;
                    }

                    ndof=1;
                }
                else
                {
                    ndof++;
                }
            }
        }

        if(old_ndof == -1)
            mndof = 1;
        else
            mndof = ndof;
        //KRATOS_WATCH(mndof);
    }

private:

    double mTol;
    int mmax_it;
    int mincomplete;
    int mlevel_of_fill;
    int mverbosity;
    int mndof;
    int msymmetric;
//     double mDropTol;
//     double mFillTol;
//     double mFillFactor;

    /**
     * Assignment operator.
     */
    PastixSolver& operator=(const PastixSolver& Other);

    /**
     * Copy constructor.
     */
    PastixSolver(const PastixSolver& Other);

}; // Class PastixSolver

}  // namespace Kratos.

#endif // KRATOS_PastixSolver  defined
