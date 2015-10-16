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
#include <memory>

#include <iostream>

template <typename T, size_t fD, size_t mD>
class Field
{
    using vectorField = Field<T,mD,mD>;
    using scalarField = Field<T,1,mD>;
    using MeshPtr = std::shared_ptr<const Mesh<mD>>;

public:
    // Empty value constructor
    Field(const MeshPtr mesh, const std::string& name):
        Field(mesh, name, std::make_index_sequence<fD>{})
    {}

// ------ Copy, move, destructor calls need declaring and defining ---------
    Field(const Field<T, fD, mD> &rhs, const std::string& name = std::string()):
        mesh_(rhs.meshPtr()),
        numCells_(rhs.numCells()),
        xCells_(rhs.xCells_), yCells_(rhs.yCells_), zCells_(rhs.zCells_),
        name_(name.empty() ? rhs.name() : name),
        valueArray_(rhs.data())
    {}

    Field(Field<T, fD, mD> &&rhs): Field<T,fD,mD>() {
        swap(*this, rhs);
    }

    ~Field() = default;

    Field<T,fD,mD>& operator=(Field<T,fD,mD> rhs) {
        swap(*this, rhs);
        return *this;
    }

    friend void swap(Field<T,fD,mD>& first, Field<T,fD,mD>& second) {
        using std::swap;
        swap(first.mesh_, second.mesh_);
//        swap(const_cast<size_t>(first.numCells_),
//             const_cast<size_t>(second.numCells_));
        swap(first.numCells_, second.numCells_);
        swap(first.xCells_, second.xCells_);
        swap(first.yCells_, second.yCells_);
        swap(first.zCells_, second.zCells_);
        swap(first.name_, second.name_);
        swap(first.valueArray_, second.valueArray_);
    }
// -------------- Copy, move, assignment and destructor calls --------------


    // Compound mathematical operators
    const Field<T,fD,mD> operator+=(const T& rhs) {
        for (std::vector<T>& vec : valueArray_) {
            for (T& v : vec) {
                v += rhs;
            }
        }
        return *this;
    }
    const Field<T,fD,mD> operator-=(const T& rhs) {
        return this->operator+=(-rhs);
    }
    const Field<T,fD,mD> operator*=(const T& rhs) {
        for (std::vector<T>& vec : valueArray_) {
            for (T& v : vec) {
                v *= rhs;
            }
        }
        return *this;
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
        if (*mesh_ != rhs.mesh()) {
            return false;
        }
        for (size_t d=0; d<fD; d++) {
            if (valueArray_[d] != rhs.data()[d]) {
                return false;
            }
        }
        return true;
    }
    bool equal_Val_Name(const Field<T,fD,mD>& rhs) {
        if (name_ != rhs.name_) {
            return false;
        }
        return operator==(rhs);
    }
    bool operator!=(const Field<T,fD,mD>& rhs) {
        return ! operator==(rhs);
    }
    bool strictlyEqual(const Field<T,fD,mD>& rhs) {
        return (this==(&rhs));
    }

    // Value lookup (every cell, one component)
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

    // Individual lookups (probably slow, mark deprecated?)
    // Should these check the type of Idxs?
    template<typename... Idxs, EnableIf<sizeof...(Idxs)==mD>...>
    [[deprecated]] const T x(const Idxs... idxs) const {
        return valueArray_[0][getSingleIdx(idxs...)];
    }

    // Other lookups
    size_t numCells() const { return numCells_; }
    const Mesh<mD> &mesh() const { return *mesh_; }
    MeshPtr meshPtr() const { return mesh_; }
    const std::string &name() const { return name_; }
    std::array<std::vector<T>, fD> data() { return valueArray_; }
    const std::array<std::vector<T>, fD> data() const { return valueArray_; }

private:
    // Data members
    std::shared_ptr<const Mesh<mD>> mesh_;
    /*const*/ size_t numCells_;
    size_t xCells_, yCells_, zCells_;
    std::string name_;
    std::array<std::vector<T>, fD> valueArray_;
    // End data members

    template<typename... Idxs, EnableIf<sizeof...(Idxs)==mD>...>
    [[deprecated]] size_t getSingleIdx(const Idxs... idxs) const {
        std::vector<size_t> idx { idxs... };
        // For <=3D meshes, will never cut out an index - just pad with 0s.
        idx.resize(3);
        return idx[0] + (idx[1]*xCells_) + (idx[2]*xCells_*yCells_);
    }


// --------------- Delegated constructors ---------------------------------- //
    // Delegated empty but appropriately sized constructor
    template<size_t... Is>
    Field(const MeshPtr mesh, const std::string& name, std::index_sequence<Is...>):
        mesh_(mesh),
        numCells_(mesh->numCells()),
        xCells_(mesh->xCells()),
        yCells_(mesh->yCells()),
        zCells_(mesh->zCells()),
        name_(name),
        valueArray_{ { std::vector<T>(((void)Is, numCells_))... } }
    {}

    // Default constructor with appropriate number of empty vector<T>s
    Field():
        Field(std::make_index_sequence<fD>{})
    {}

    // Delegated empty constructor
    template<size_t... Is>
    Field(std::index_sequence<Is...>):
        mesh_(nullptr),
        numCells_(),
        xCells_(),
        yCells_(),
        zCells_(),
        name_(""),
        valueArray_{{ std::vector<T>(((void)Is, 0))... }}
    {}
// --------------- End delegated constructors ------------------------------ //
};

// Mathematical operators that don't change the Field
template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator+(Field<T,fD,mD> lhs,
                               const typename std::common_type<T>::type &rhs)
{
    return lhs += rhs;
}

template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator+(const typename std::common_type<T>::type &lhs,
                               Field<T,fD,mD> rhs)
{
    return rhs += lhs;
}

template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator-(Field<T,fD,mD> lhs,
                               const typename std::common_type<T>::type &rhs)
{
    return lhs -= rhs;
}

template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator-(const typename std::common_type<T>::type &lhs,
                               Field<T,fD,mD> rhs)
{
    return (-1 * rhs) += lhs;
}

template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator*(Field<T,fD,mD> lhs,
                               const typename std::common_type<T>::type &rhs)
{
    return lhs *= rhs;
}

template<typename T, size_t fD, size_t mD>
const Field<T,fD,mD> operator*(const typename std::common_type<T>::type &lhs,
                               Field<T,fD,mD> rhs)
{
    return rhs *= lhs;
}

#endif // DATASTRUCTURES_FIELD_TPP
