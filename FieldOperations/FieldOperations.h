#ifndef FIELD_OPERATIONS_H
#define FIELD_OPERATIONS_H

#include "Field.tpp"
#include <cstddef>

namespace FieldOperations
{
    template<typename T, size_t fD, size_t mD>
    Field<T,fD,mD> operator+(Field<T,fD,mD> lhs, const Field<T,fD,mD> &rhs);/* {
        return lhs;
    }*/
}

#endif // FIELD_OPERATIONS_H
