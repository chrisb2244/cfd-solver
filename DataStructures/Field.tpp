/* ---------------------------------------------------------------------------
 * Basic Field class.
 *
 * Templated by data type T (eg double)
 *              the size_t fD (field dimension, expected to be 1 or mD)
 *              and size_t mD (mesh dimension)
 *
 *
 * --------------------------------------------------------------------------*/

#ifndef DATASTRUCTURES_FIELD_TPP
#define DATASTRUCTURES_FIELD_TPP

#include <cstddef>
#include <vector>
#include <array>
#include "Mesh.h"
#include <utility>

template <typename T, size_t fD, size_t mD>
class Field
{
    using vectorField = Field<T,mD,mD>;
    using scalarField = Field<T,1,mD>;

public:
    // Empty value constructor
    Field(const Mesh<mD> &mesh, const std::string& name):
        Field(mesh, name, std::make_index_sequence<fD>{})
    {}

    // Copy, move, destructor calls need declaring and defining

    // Mathematical operators - should these be a part of Field?
    const Field<T,fD,mD> operator+(const T& rhs) const {
        Field<T,fD,mD> temp(*this);
        for (auto& vec : temp.data()) {
            for (auto& v : vec) {
                v += rhs;
            }
        }
        return temp;
    }
    const Field<T,fD,mD> operator-(const T& rhs) const {
        return (*this->operator +(-rhs));
    }

    // Set values unilaterally.
    void setZero() { setFixed(T()); }
    void setFixed(const T &val) {
        for (size_t d = 0; d<mD; d++) {
            std::fill(valueArray_[d].begin(), valueArray_[d].end(), val);
        }
    }

    // Test equality
    bool operator==(const Field<T,fD,mD>& rhs) {
        if (mesh_ != rhs.mesh()) {
            return false;
        }
        for (size_t d=0; d<fD; d++) {
            if (valueArray_[d] != rhs.data()[d]) {
                return false;
            }
        }
        return true;
    }
    bool strictlyEqual(const Field<T,fD,mD>& rhs) {
        if (name_ != rhs.name_) {
            return false;
        }
        return operator==(rhs);
    }
    bool operator!=(const Field<T,fD,mD>& rhs) {
        return ! operator==(rhs);
    }

    // Value lookup
    const std::vector<T>& x() const { return valueArray_[0]; }
    template<size_t md=mD, EnableIf<md>=2>...>
    const std::vector<T> &y() const { return valueArray_[1]; }
    template<size_t md=mD, EnableIf<md>=3>...>
    const std::vector<T> &z() const { return valueArray_[2]; }
    // Non-const
    std::vector<T> &x() { return valueArray_[0]; }
    template<size_t md=mD, EnableIf<md>=2>...>
    std::vector<T> &y() { return valueArray_[1]; }
    template<size_t md=mD, EnableIf<md>=3>...>
    std::vector<T> &z() { return valueArray_[2]; }

    // Other lookups
    size_t numCells() const { return numCells_; }
    const Mesh<mD> &mesh() const { return mesh_; }
    std::array<std::vector<T>, fD> data() { return valueArray_; }
    const std::array<std::vector<T>, fD> data() const { return valueArray_; }

private:
    const Mesh<mD> &mesh_;
    const size_t numCells_;
    const std::string name_;

    std::array<std::vector<T>, fD> valueArray_;

    // Empty constructor (e.g. for move constructor)
    Field(const Mesh<mD> &mesh) noexcept:
        mesh_(mesh),
        name_("toDelete") {}

    // Sub-constructor for delegation
    template<size_t... Is>
    Field(const Mesh<mD> &mesh, const std::string& name, std::index_sequence<Is...>):
        mesh_(mesh),
        numCells_(mesh.numCells()),
        name_(name),
        valueArray_{ { std::vector<T>(((void)Is, numCells_))... } }
    {}
};



#endif // DATASTRUCTURES_FIELD_TPP
