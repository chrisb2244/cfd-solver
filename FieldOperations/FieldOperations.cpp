#include "FieldOperations.h"

template<typename T, size_t fD, size_t mD>
Field<T, fD, mD> FieldOperations::operator+(Field<T,fD,mD> lhs, const Field<T,fD,mD>& rhs) {
    return lhs+=rhs;
}

