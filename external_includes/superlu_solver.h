/*
* =======================================================================*
* kkkk   kkkk  kkkkkkkkkk   kkkkk    kkkkkkkkkk kkkkkkkkkk kkkkkkkkkK    *
* kkkk  kkkk   kkkk   kkkk  kkkkkk   kkkkkkkkkk kkkkkkkkkk kkkkkkkkkK    *
* kkkkkkkkk    kkkk   kkkk  kkkkkkk     kkkk    kkk    kkk  kkkk         *
* kkkkkkkkk    kkkkkkkkkkk  kkkk kkk    kkkk    kkk    kkk    kkkk       *
* kkkk  kkkk   kkkk  kkkk   kkkk kkkk   kkkk    kkk    kkk      kkkk     *
* kkkk   kkkk  kkkk   kkkk  kkkk  kkkk  kkkk    kkkkkkkkkk  kkkkkkkkkk   *
* kkkk    kkkk kkkk    kkkk kkkk   kkkk kkkk    kkkkkkkkkk  kkkkkkkkkk      *
*                                                                        *
* krATos: a fREe opEN sOURce CoDE for mULti-pHysIC aDaptIVe SoLVErS,     *
* aN extEnsIBLe OBjeCt oRiEnTEd SOlutION fOR fInITe ELemEnt fORmULatIONs *
* Copyleft by 2003 ciMNe                                                 *
* Copyleft by 2003 originary authors Copyleft by 2003 your name          *
* This library is free software; you can redistribute it and/or modify   *
* it under the terms of the GNU Lesser General Public License as         *
* published by the Free Software Foundation; either version 2.1 of       *
* the License, or any later version.                                     *
*                                                                        *
* This library is distributed in the hope that it will be useful, but    *
* WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
* See the GNU Lesser General Public License for more details.            *
*                                                                        *
* You should have received a copy of the GNU Lesser General Public       *
* License along with this library; if not, write to International Centre *
* for Numerical Methods in Engineering (CIMNE),                          *
* Edifici C1 - Campus Nord UPC, Gran Capità s/n, 08034 Barcelona.        *
*                                                                        *
* You can also contact us to the following email address:                *
* kratos@cimne.upc.es                                                    *
* or fax number: +34 93 401 65 17                                        *
*                                                                        *
* Created at Institute for Structural Mechanics                          *
* Ruhr-University Bochum, Germany                                        *
* Last modified by:    $Author: hbui $                   *
* Date:                $Date: 21 Jul 2018 $             *
* Revision:            $Revision: 1.6 $                  *
*========================================================================*
* International Center of Numerical Methods in Engineering - CIMNE     *
* Barcelona - Spain                              *
*========================================================================*
*/

#if !defined(KRATOS_SUPERLU_SOLVER_H_INCLUDED )
#define  KRATOS_SUPERLU_SOLVER_H_INCLUDED

// External includes
#include "boost/smart_ptr.hpp"

#include "includes/ublas_interface.h"
// #include "external_includes/superlu/superlu.hpp"

#include "SRC/slu_ddefs.h"

// Project includes
#include "includes/define.h"
#include "linear_solvers/direct_solver.h"

namespace ublas = boost::numeric::ublas;

namespace Kratos
{
template< class TSparseSpaceType, class TDenseSpaceType,
          class TModelPartType,
          class TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType> >
class SuperLUSolver : public DirectSolver< TSparseSpaceType,
    TDenseSpaceType, TModelPartType, TReordererType>
{
public:
    /**
     * Counted pointer of SuperLUSolver
     */
    KRATOS_CLASS_POINTER_DEFINITION(  SuperLUSolver );

    typedef DirectSolver<TSparseSpaceType, TDenseSpaceType, TModelPartType, TReordererType> BaseType;

    typedef typename TSparseSpaceType::MatrixType SparseMatrixType;

    typedef typename TSparseSpaceType::VectorType VectorType;

    typedef typename TDenseSpaceType::MatrixType DenseMatrixType;

    typedef typename BaseType::ModelPartType ModelPartType;

    /**
     * Default constructor
     */
    SuperLUSolver() {}

    /**
     * Destructor
     */
    ~SuperLUSolver() override {}

    bool AdditionalPhysicalDataIsNeeded() override
    {
        return false;
    }

    void ProvideAdditionalData(
        SparseMatrixType& rA,
        VectorType& rX,
        VectorType& rB,
        typename ModelPartType::DofsArrayType& rdof_set,
        ModelPartType& r_model_part
    ) override
    {}

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
        // std::cout << "matrix size in solver:  " << rA.size1() << std::endl;
        // std::cout << "RHS size in solver SLU: " << rB.size() << std::endl;

//               typedef ublas::compressed_matrix<double, ublas::row_major, 0,
//                 ublas::unbounded_array<int>, ublas::unbounded_array<double> > cm_t;

        //make a copy of the RHS
        VectorType rC=rB;

        if(this->IsNotConsistent(rA, rX, rB))
            return false;

        superlu_options_t options;
        SuperLUStat_t stat;

        /* Set the default input options:
            options.Fact = DOFACT;
            options.Equil = YES;
            options.ColPerm = COLAMD;
            options.DiagPivotThresh = 1.0;
            options.Trans = NOTRANS;
            options.IterRefine = NOREFINE;
            options.SymmetricMode = NO;
            options.PivotGrowth = NO;
            options.ConditionNumber = NO;
            options.PrintStat = YES;
        */
        set_default_options(&options);
        options.IterRefine = SLU_DOUBLE;
//         options.ColPerm = MMD_AT_PLUS_A;

        //Fill the SuperLU matrices
        SuperMatrix Aslu, B, L, U;

        //create a copy of the matrix
        std::vector<int> index1_vector(rA.index1_data().size());
        std::vector<int> index2_vector(rA.index2_data().size());
//         double *values_vector = new (std::nothrow) double[rA.value_data().size()];

        for( int unsigned i = 0; i < rA.index1_data().size(); i++ )
            index1_vector[i] = (int)rA.index1_data()[i];

        for( unsigned int i = 0; i < rA.index2_data().size(); i++ )
            index2_vector[i] = (int)rA.index2_data()[i];

        /*        for( unsigned int i = 0; i < rA.value_data().size(); i++ )
                    values_vector[i] = (double)rA.value_data()[i];*/

        //create a copy of the rhs vector (it will be overwritten with the solution)
        /*        double *b_vector = new (std::nothrow) double[rB.size()];
                for( unsigned int i = 0; i < rB.size(); i++ )
                    b_vector[i] = rB[i];*/
        /*
                dCreate_CompCol_Matrix (&Aslu, rA.size1(), rA.size2(),
                                   rA.nnz(),
                                  values_vector,
                                  index2_vector,
                                   index1_vector,
                                  SLU_NR, SLU_D, SLU_GE
                                  );*/

        //works also with dCreate_CompCol_Matrix
        dCreate_CompRow_Matrix (&Aslu, rA.size1(), rA.size2(),
                                rA.nnz(),
                                rA.value_data().begin(),
                                index2_vector.data(), //can not avoid a copy as ublas uses unsigned int internally
                                index1_vector.data(), //can not avoid a copy as ublas uses unsigned int internally
                                SLU_NR, SLU_D, SLU_GE
                               );

        dCreate_Dense_Matrix (&B, rB.size(), 1,&rB[0],rB.size(),SLU_DN, SLU_D, SLU_GE);

        //allocate memory for permutation arrays
        int* perm_c;
        int* perm_r;
        if ( !(perm_c = intMalloc(rA.size1())) ) ABORT("Malloc fails for perm_c[].");
        if ( !(perm_r = intMalloc(rA.size2())) ) ABORT("Malloc fails for perm_r[].");


        //initialize container for statistical data
        StatInit(&stat);

        //call solver routine
        int info;
        dgssv(&options, &Aslu, perm_c, perm_r, &L, &U, &B, &stat, &info);

        //print output
        if (options.PrintStat) {
        StatPrint(&stat);
        }

        //resubstitution of results
        #pragma omp parallel for
        for(int i=0; i<static_cast<int>(rB.size()); i++ )
            rX[i] = rB[i]; // B(i,0);

        //recover the RHS
        rB=rC;

        //deallocate memory used
        StatFree(&stat);
        SUPERLU_FREE (perm_r);
        SUPERLU_FREE (perm_c);
        Destroy_SuperMatrix_Store(&Aslu); //note that by using the "store" function we will take care of deallocation ourselves
        Destroy_SuperMatrix_Store(&B);
        Destroy_SuperNode_Matrix(&L);
        Destroy_CompCol_Matrix(&U);

//         delete [] b_vector;

        //CHECK WITH VALGRIND IF THIS IS NEEDED ...or if it is done by the lines above
        //deallocate tempory storage used for the matrix
//                 if(b_vector!=NULL) delete [] index1_vector;
// //           if(b_vector!=NULL) delete [] index2_vector;
//           if(b_vector!=NULL) delete [] values_vector;
//         if(b_vector!=NULL) delete [] b_vector;

        return true;
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
        //std::cout << "matrix size in solver:  " << rA.size1() << std::endl;
        //std::cout << "RHS size in solver SLU: " << rB.size() << std::endl;

//               typedef ublas::compressed_matrix<double, ublas::row_major, 0,
//                 ublas::unbounded_array<int>, ublas::unbounded_array<double> > cm_t;

        if(this->IsNotConsistent(rA, rX, rB))
            return false;

        superlu_options_t options;
        SuperLUStat_t stat;

        /* Set the default input options:
            options.Fact = DOFACT;
            options.Equil = YES;
            options.ColPerm = COLAMD;
            options.DiagPivotThresh = 1.0;
            options.Trans = NOTRANS;
            options.IterRefine = NOREFINE;
            options.SymmetricMode = NO;
            options.PivotGrowth = NO;
            options.ConditionNumber = NO;
            options.PrintStat = YES;
        */
        set_default_options(&options);
        options.IterRefine = SLU_DOUBLE;
//         options.ColPerm = MMD_AT_PLUS_A;

        //Fill the SuperLU matrices
        SuperMatrix Aslu, B, L, U;

        //create a copy of the matrix
        std::vector<int> index1_vector(rA.index1_data().size());
        std::vector<int> index2_vector(rA.index2_data().size());
//         double *values_vector = new (std::nothrow) double[rA.value_data().size()];

        for( int unsigned i = 0; i < rA.index1_data().size(); i++ )
            index1_vector[i] = (int)rA.index1_data()[i];

        for( unsigned int i = 0; i < rA.index2_data().size(); i++ )
            index2_vector[i] = (int)rA.index2_data()[i];

        /*        for( unsigned int i = 0; i < rA.value_data().size(); i++ )
                    values_vector[i] = (double)rA.value_data()[i];*/

        //create a copy of the rhs vector (it will be overwritten with the solution)
        /*        double *b_vector = new (std::nothrow) double[rB.size()];
                for( unsigned int i = 0; i < rB.size(); i++ )
                    b_vector[i] = rB[i];*/
        /*
                dCreate_CompCol_Matrix (&Aslu, rA.size1(), rA.size2(),
                                   rA.nnz(),
                                  values_vector,
                                  index2_vector,
                                   index1_vector,
                                  SLU_NR, SLU_D, SLU_GE
                                  );*/

        //works also with dCreate_CompCol_Matrix
        dCreate_CompRow_Matrix (&Aslu, rA.size1(), rA.size2(),
                                rA.nnz(),
                                rA.value_data().begin(),
                                index2_vector.data(), //can not avoid a copy as ublas uses unsigned int internally
                                index1_vector.data(), //can not avoid a copy as ublas uses unsigned int internally
                                SLU_NR, SLU_D, SLU_GE
                               );

        std::vector<double> dB(rB.size1()*rB.size2());
        for (unsigned int j = 0; j < rB.size2(); ++j)
            for (unsigned int i = 0; i < rB.size1(); ++i)
                dB[j*rB.size1()+i] = rB(i, j);
        dCreate_Dense_Matrix (&B, rB.size1(), rB.size2(), &dB[0], rB.size1(), SLU_DN, SLU_D, SLU_GE);

        //allocate memory for permutation arrays
        int* perm_c;
        int* perm_r;
        if ( !(perm_c = intMalloc(rA.size1())) ) ABORT("Malloc fails for perm_c[].");
        if ( !(perm_r = intMalloc(rA.size2())) ) ABORT("Malloc fails for perm_r[].");


        //initialize container for statistical data
        StatInit(&stat);

        //call solver routine
        int info;
        dgssv(&options, &Aslu, perm_c, perm_r, &L, &U, &B, &stat, &info);

        //print output
        if (options.PrintStat) {
        StatPrint(&stat);
        }

        //resubstitution of results
        #pragma omp parallel for
#ifdef _MSC_VER
        for (int j = 0; j < rB.size2(); ++j)
            for (int i = 0; i < rB.size1(); ++i)
#else
        for (unsigned int j = 0; j < rB.size2(); ++j)
            for (unsigned int i = 0; i < rB.size1(); ++i)
#endif
                rX(i, j) = dB[j*rB.size1()+i];

        //deallocate memory used
        StatFree(&stat);
        SUPERLU_FREE (perm_r);
        SUPERLU_FREE (perm_c);
        Destroy_SuperMatrix_Store(&Aslu); //note that by using the "store" function we will take care of deallocation ourselves
        Destroy_SuperMatrix_Store(&B);
        Destroy_SuperNode_Matrix(&L);
        Destroy_CompCol_Matrix(&U);

//         delete [] b_vector;

        //CHECK WITH VALGRIND IF THIS IS NEEDED ...or if it is done by the lines above
        //deallocate tempory storage used for the matrix
//                 if(b_vector!=NULL) delete [] index1_vector;
// //           if(b_vector!=NULL) delete [] index2_vector;
//           if(b_vector!=NULL) delete [] values_vector;
//         if(b_vector!=NULL) delete [] b_vector;

        return true;
    }

    /**
     * Print information about this object.
     */
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "SuperLU solver finished.";
    }

    /**
     * Print object's data.
     */
    void PrintData(std::ostream& rOStream) const override
    {
    }

private:

    /**
     * Assignment operator.
     */
    SuperLUSolver& operator=(const SuperLUSolver& Other);

    /**
     * Copy constructor.
     */
    SuperLUSolver(const SuperLUSolver& Other);

}; // Class SkylineLUFactorizationSolver

}  // namespace Kratos.

#endif // KRATOS_SUPERLU_SOLVER_H_INCLUDED  defined
