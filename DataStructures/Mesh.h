/* ------------------------------------------------------------------------- *\
|   The Mesh class should have only one object
|   This is NOT enforced by the Mesh class
|
|   Once constructed (by main.cpp?) it should not be changed - it cannot be
|       restructured, and cells cannot be moved or refined or unrefined
|
\* ------------------------------------------------------------------------- */

#ifndef MESH_H
#define MESH_H

#include <type_traits>
#include <vector>
#include <cstddef>

// #define NDEBUG // Uncomment to disable asserts
#include <cassert>

#include "TemplateFunctions.H"

struct MeshDimension
{
    constexpr MeshDimension(const int numCells, const double min, const double max):
        numCells_(numCells),
        minVal_(min),
        maxVal_(max)
    {}

    // Prevent implicit cast from double to int for 'numCells'
    MeshDimension(double, double, double) = delete;

    const int numCells_;
    const double minVal_, maxVal_;
};

enum class MeshScalingType {
    Constant,
    Pivot,
    Hyperbolic,
    Exponential
};

template<size_t meshDim>
class Mesh
{
    using DimList = std::vector<MeshDimension>;

public:
    // Constructor
    template<typename... Dims,
             EnableIf<areT<MeshDimension, Dims...>::value>...,
             EnableIf<sizeof...(Dims)==meshDim>...>
    Mesh(MeshScalingType scaling, Dims... dims):
        scalingType_(scaling)
    {
        DimList dimList{dims...};
        for (size_t i=0; i<meshDim; i++) {
            dimSize_.push_back(dimList[i].numCells_);
            dimMin_.push_back(dimList[i].minVal_);
            dimMax_.push_back(dimList[i].maxVal_);
            numCells_ = calcNumCells();
            placeCentres(i);
        }
    }

    // Copy and move constructors
    Mesh(const Mesh<meshDim>& rhs):
        dimMin_(rhs.dimMin()),
        dimMax_(rhs.dimMax()),
        dimSize_(rhs.dimSizes()),
        numCells_(rhs.numCells()),
        scalingType_(rhs.scalingType()),
        position_(rhs.position_)
    {}

    Mesh(Mesh<meshDim>&& rhs):
        Mesh()
    {
        swap(*this, rhs);
    }

    // Copy/move assignment operator
    Mesh<meshDim>& operator=(Mesh<meshDim> rhs) {
        swap(*this, rhs);
        return *this;
    }

    friend void swap(Mesh<meshDim>& first, Mesh<meshDim>& second) {
        using std::swap;
        swap(first.dimMin_, second.dimMin_);
        swap(first.dimMax_, second.dimMax_);
        swap(first.dimSize_, second.dimSize_);
        swap(first.numCells_, second.numCells_);
        swap(first.scalingType_, second.scalingType_);
        swap(first.position_, second.position_);
    }

    static Mesh<meshDim> dummyMesh() {
        return Mesh();
    }

    size_t numCells() const { return numCells_; }
    // Always defined (can't have a 0D mesh)
    size_t xCells() const { return dimSize_[0]; }
    // Get size if 2D or greater, return 0 otherwise
    template<size_t meshD = meshDim, EnableIf<meshD>=2>...>
    size_t yCells() const { return dimSize_[1]; }
    template<size_t meshD = meshDim, EnableIf<meshD<2>...>
    size_t yCells() const { return 0; }
    // Get size if 3D or greater, return 0 otherwise
    template<size_t meshD = meshDim, EnableIf<meshD>=3>...>
    size_t zCells() const { return dimSize_[2]; }
    template<size_t meshD = meshDim, EnableIf<meshD<3>...>
    size_t zCells() const { return 0; }

//    double x(const size_t &i) const { return getPosition(0, i); }
//    template<size_t meshD = meshDim, EnableIf<meshD>=2>...>
//    double y(const size_t &i) const { return getPosition(1, i); }
//    template<size_t meshD = meshDim, EnableIf<meshD>=3>...>
//    double z(const size_t &i) const { return getPosition(2, i); }

//    // This can return a Vector if preferred.
//    template<typename... I,
//             EnableIf<areT<int, I...>::value>...,
//             EnableIf<sizeof...(I)==meshDim>...>
//    const std::vector<double> cellCentre(I... indices) const;

    // Comparison operators
    template<size_t rhsDim>
    bool operator==(const Mesh<rhsDim> &) const { return false; }
    bool operator==(const Mesh<meshDim> &rhs) const;
    template<size_t rhsDim>
    bool isSameMesh(const Mesh<rhsDim> &) const { return false; }
    bool isSameMesh(const Mesh<meshDim> &rhs) const;
    template<size_t rhsDim>
    bool operator!=(const Mesh<rhsDim> &rhs) const { return ! operator==(rhs); }

    const std::vector<double>& dimMin() const { return dimMin_; }
    const std::vector<double>& dimMax() const { return dimMax_; }
    const std::vector<size_t>& dimSizes() const { return dimSize_; }
    const MeshScalingType& scalingType() const { return scalingType_; }

private:
    // Data members
    std::vector<double> dimMin_;
    std::vector<double> dimMax_;
    std::vector<size_t> dimSize_;
    size_t numCells_;
    MeshScalingType scalingType_;
    std::vector<double> position_[meshDim];
    // End data members

    Mesh():
        dimMin_(meshDim),
        dimMax_(meshDim),
        dimSize_(meshDim),
        numCells_(),
        scalingType_(MeshScalingType::Constant)
    {
        for (size_t d=0; d<meshDim; d++) {
            position_[d] = std::vector<double>();
        }
    }

    void placeCentres(const int d);
    size_t calcNumCells() const;

    double getPosition(const size_t d, const int &i) const;

    void checkBounds(std::vector<size_t> idxs) const;
};

//class dummyMesh:
//    public Mesh<1>
//{
//public:
//    dummyMesh():
//        Mesh(MeshScalingType::Constant, MeshDimension(1,0,1))
//    {}
//};

#endif // MESH_H
