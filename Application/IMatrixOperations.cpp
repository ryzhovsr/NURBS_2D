#include "imatrixoperations.h"
#include "eigenmatrixoperations.h"

IMatrixOperationsPtr IMatrixOperations::GetMatrixOperationsClass(OperationClass className)
{
    switch (className)
    {
    case eigen:
        return std::make_shared<EigenMatrixOperations>();
    /*
    case mkl:
        return std::make_shared<MKLMatrixOperations>();
    */
    default:
        return nullptr;
    }
}
