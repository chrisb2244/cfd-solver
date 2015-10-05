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

#include <iostream>

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

// ------ Copy, move, destructor calls need declaring and defining ---------
    Field(const Field<T,fD,mD>& rhs):
        mesh_(rhs.mesh()),
        numCells_(rhs.numCells()),
        name_(rhs.name()),
        valueArray_(rhs.data())
    {}

    Field(const Field<T, fD, mD> &rhs, const std::string& name):
        mesh_(rhs.mesh()),
        numCells_(rhs.numCells()),
        name_(name),
        valueArray_(rhs.data())
    {}

    Field(Field<T, fD, mD> &&rhs):
        Field<T,fD,mD>(Mesh<mD>::dummyMesh())
    {
        swap(*this, rhs);
    }

//        mesh_(rhs.mesh()),
//        numCells_(rhs.numCells()),
//        name_(rhs.name())
//    {
//        for (size_t d=0; d<fD; d++) {
//            valueArray_[d] = std::move(rhs.data()[d]);
//        }
//        std::cout << "Moved" << std::endl;
//    }

    ~Field() = default;

    Field<T,fD,mD>& operator=(Field<T,fD,mD> rhs) {
        swap(*this, rhs);
        return *this;
    }

    friend void swap(Field<T,fD,mD>& first, Field<T,fD,mD>& second) {
        using std::swap;
        swap(const_cast<Mesh<mD>&>(first.mesh_),
             const_cast<Mesh<mD>&>(second.mesh_));
//        swap(first.mesh_, second.mesh_);

//        swap(const_cast<size_t>(first.numCells_),
//             const_cast<size_t>(second.numCells_));
        swap(first.numCells_, second.numCells_);

        swap(first.name_, second.name_);
        swap(first.valueArray_, second.valueArray_);
    }

// -------------- Copy, move, assignment and destructor calls --------------

    // Mathematical operators - should these be a part of Field?
    const Field<T,fD,mD> operator+(const T& rhs) const {
        Field<T,fD,mD> temp(*this);
        for (std::vector<T>& vec : temp.data()) {
            for (T& v : vec) {
                std::cout << "v = " << v;
                v += rhs;
                std::cout << " -> v = " << v << std::endl;
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
            std::cout << "mesh failed" << std::endl;
            return false;
        }
        for (size_t d=0; d<fD; d++) {
            if (valueArray_[d] != rhs.data()[d]) {
                std::cout << "vec[" << d << "] failed" << std::endl;
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
    const std::string &name() const { return name_; }
    std::array<std::vector<T>, fD> data() { return valueArray_; }
    const std::array<std::vector<T>, fD> data() const { return valueArray_; }

private:
    // Data members
    const Mesh<mD> &mesh_;
    /*const*/ size_t numCells_;
    std::string name_;
    std::array<std::vector<T>, fD> valueArray_;
    // End data members

    // Sub-constructor for delegation
    template<size_t... Is>
    Field(const Mesh<mD> &mesh, const std::string& name, std::index_sequence<Is...>):
        mesh_(mesh),
        numCells_(mesh.numCells()),
        name_(name),
        valueArray_{ { std::vector<T>(((void)Is, numCells_))... } }
    {}

    Field(Mesh<mD> dummy):
        mesh_(dummy),
        numCells_(1),
        name_(""),
        valueArray_(std::array<std::vector<T>,1>{std::vector<T>{0}})
    {}
};



#endif // DATASTRUCTURES_FIELD_TPP
