//
//   Project Name:        Kratos
//   Last modified by:    $Author: janosch $
//   Date:                $Date: 2008-07-23 14:47:45 $
//   Revision:            $Revision: 1.3 $
//
//

// System includes

#if defined(KRATOS_PYTHON)
// External includes


// Project includes
#include "includes/define_python.h"
#include "externalsolvers_application.h"
#include "custom_python/add_linear_solvers_to_python.h"


namespace Kratos
{

namespace Python
{

BOOST_PYTHON_MODULE(KratosExternalSolversApplication)
{

    using namespace boost::python;

    class_<KratosExternalSolversApplication,
           KratosExternalSolversApplication::Pointer,
           bases<KratosApplication>, boost::noncopyable >("KratosExternalSolversApplication")
           ;

    AddLinearSolversToPython();

}

}  // namespace Python.

}  // namespace Kratos.

#endif // KRATOS_PYTHON defined
