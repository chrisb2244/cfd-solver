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
#include "BoundingBox.h"

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

template<size_t n>
struct helper
{
    typedef std::size_t type;
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
            placeEdges(i);
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
        centrePosition_(rhs.centrePosition_),
        edgePosition_(rhs.edgePosition_)
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
        swap(first.centrePosition_, second.centrePosition_);
        swap(first.edgePosition_, second.edgePosition_);
    }

    static Mesh<meshDim> dummyMesh() { return Mesh(); }

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

    // BoundingBox for the entire Mesh
    BoundingBox<meshDim> bounds() const;

    // BoundingBox for a specific cell
    template<size_t mD = meshDim, EnableIf<mD==1>...>
    BoundingBox<meshDim> bounds(const size_t idx) const {
        std::vector<double> bnds;
        bnds.push_back(edgePosition_[0][idx]);
        bnds.push_back(edgePosition_[0][idx+1]);
        return BoundingBox<meshDim>(std::move(bnds));
    }

    template<size_t mD = meshDim, EnableIf<mD>=2>...>
    BoundingBox<meshDim> bounds(const size_t idx) const {
        // Convert single index to set of indices
        std::vector<size_t> idcs(meshDim);
        getSubIdx(idcs, idx, helper<mD>());
        std::vector<double> bnds;
        for (size_t d=0; d<meshDim; d++) {
            bnds.push_back(edgePosition_[d][idcs[d]]);
            bnds.push_back(edgePosition_[d][idcs[d]+1]);
        }
        return BoundingBox<meshDim>(std::move(bnds));
    }

    template<typename... Indices,
             size_t mD = meshDim,
             EnableIf<mD>=2>...,
             EnableIf<sizeof...(Indices)==mD>...>
    BoundingBox<meshDim> bounds(const Indices... indices) const {
         std::vector<double> bnds;
         std::vector<size_t> idx { indices... };
         for (size_t d=0; d<meshDim; d++) {
             bnds.push_back(edgePosition_[d][idx[d]]);
             bnds.push_back(edgePosition_[d][idx[d]+1]);
         }
         return BoundingBox<meshDim>(std::move(bnds));
    }

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
    std::vector<double> centrePosition_[meshDim];
    std::vector<double> edgePosition_[meshDim];
    // End data members

    Mesh():
        dimMin_(meshDim),
        dimMax_(meshDim),
        dimSize_(meshDim),
        numCells_(),
        scalingType_(MeshScalingType::Constant)
    {
        for (size_t d=0; d<meshDim; d++) {
            centrePosition_[d] = std::vector<double>();
            edgePosition_[d] = std::vector<double>();
        }
    }

    void placeCentres(const int d);
    void placeEdges(const int d);
    size_t calcNumCells() const;

    double getCentre(const size_t d, const size_t i) const;

    void checkBounds(std::vector<size_t> idxs) const;

    template<size_t dim_n>
    void getSubIdx(std::vector<size_t> &vec, const size_t& remainder, helper<dim_n>) const;
    void getSubIdx(std::vector<size_t> &vec, const size_t &remainder, helper<1>) const;
};

#endif // MESH_H
