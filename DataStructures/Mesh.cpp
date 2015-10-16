#include "Mesh.h"

#include <cmath>

template<size_t meshDim>
BoundingBox<meshDim> Mesh<meshDim>::bounds() const {
    std::vector<double> bnds;
    for (size_t d=0; d<meshDim; d++) {
        bnds.push_back(dimMin_[d]);
        bnds.push_back(dimMax_[d]);
    }
    return BoundingBox<meshDim>(std::move(bnds));
}

template<size_t meshDim>
bool Mesh<meshDim>::operator==(const Mesh<meshDim> &rhs) const {
    if (this == &rhs) {
        return true;
    }
    if ((dimSize_ != rhs.dimSizes())
      || (dimMin_ != rhs.dimMin())
      || (dimMax_ != rhs.dimMax())) {
        return false;
    }
    if (scalingType_ != rhs.scalingType()) {
        return false;
    }
    if (numCells_ != rhs.numCells()) {
        return false;
    }
    return true;
}

template<size_t meshDim>
bool Mesh<meshDim>::isSameMesh(const Mesh<meshDim> &rhs) const {
    if (this == &rhs) {
        return true;
    }
    return false;
}

template<size_t meshDim>
void Mesh<meshDim>::placeCentres(const int d) {
    // Place the centres at the midpoints of the cells
    const size_t N = dimSize_[d];
    centrePosition_[d].reserve(N);
    for (size_t n=0; n<N; n++) {
        centrePosition_[d].push_back(0.5*(edgePosition_[d][n]+edgePosition_[d][n+1]));
    }
}

template<size_t meshDim>
void Mesh<meshDim>::placeEdges(const int d) {
    const unsigned int numEdges = dimSize_[d] + 1;
    const double range = dimMax_[d] - dimMin_[d];

    // Reserve to save time - requires .push_back later
    edgePosition_[d].reserve(numEdges);

    switch(scalingType_)
    {
        case MeshScalingType::Constant:
        {
            // Constant spacing
            const double dx = range / dimSize_[d];
            for (size_t i=0; i<numEdges-1; i++) {
                edgePosition_[d].push_back(dimMin_[d] + (dx*static_cast<double>(i)));
            }
            // Force the edge on the last point (in case dx*N + min != max)
            edgePosition_[d].push_back(dimMax_[d]);
        }
        break;

        case MeshScalingType::Exponential:
        {
            // Exponential spacing
            constexpr double b = 3;
            constexpr double dom = exp(b)-1;
            for (size_t i=0; i<dimSize_[d]; i++) {
                const double u = static_cast<double>(i) / dimSize_[d];
                const double x = ((exp(b*u) - 1)/dom);
                edgePosition_[d].push_back((x*range) + dimMin_[d]);
            }
            edgePosition_[d].push_back(dimMax_[d]);
        }
        break;

        case MeshScalingType::Hyperbolic:
        {
            // Hyperbolic tangent spacing
            constexpr double b = 3;
            constexpr double c = 0.5; // 1-c is the location of the 'attractor'
            constexpr double A = tanh(b*-1*c);
            constexpr double B = tanh(b*(1-c));
            for (size_t i=0; i<dimSize_[d]; i++) {
                const double u = static_cast<double>(i) / dimSize_[d];
                const double x = (tanh(b*(u-c)) - A) / (B-A);
                edgePosition_[d].push_back((x*range) + dimMin_[d]);
            }
            edgePosition_[d].push_back(dimMax_[d]);
        }
        break;

        case MeshScalingType::Pivot:
        {
            edgePosition_[d].resize(numEdges);
            // Pivoted spacing - requires an even number of cells
            assert(dimSize_[d]%2==0);
            constexpr double p=0.35;
            constexpr double b = 2;
            constexpr double dom = exp(b)-1;
            const double mid = (range * p) + dimMin_[d];
            const size_t dim2 = dimSize_[d]/2;
            for (size_t i=0; i<dim2; i++) {
                const double u = static_cast<double>(i) / dim2;
                const double x = ((exp(b*u) - 1)/dom);
                const double step_lower = x*range*(p < 0.5 ? p : 1-p);
                const double step_upper = x*range*(p < 0.5 ? 1-p : p);
                // Upper half
                edgePosition_[d][i+dim2] = mid + step_upper;
                // Lower half
                edgePosition_[d][dim2-i-1] = mid - step_lower;
            }
        }
        break;
    }
}

template<size_t meshDim>
size_t Mesh<meshDim>::calcNumCells() const
{
    int numcells = 1;
    for (size_t d=0; d<meshDim; d++) {
        numcells *= dimSize_[d];
    }
    return numcells;
}

template<size_t meshDim>
double Mesh<meshDim>::getCentre(const size_t d, const size_t i) const
{
    // Bounds checking if NDEBUG not defined
    assert(i<dimSize_[d]);
    return centrePosition_[d][i];
}

template<size_t meshDim>
void Mesh<meshDim>::checkBounds(std::vector<size_t> idxs) const
{
    for (size_t d=0; d<meshDim; d++) {
        assert(idxs[d] < dimSize_[d]);
    }
}

template<size_t meshDim>
template<size_t dim_n>
void Mesh<meshDim>::getSubIdx(std::vector<size_t> &vec, const size_t &remainder, helper<dim_n>) const {
    size_t denom = 1;
    for (size_t d=0; d<dim_n-1; d++) {
        denom *= dimSize_[d];
    }
    vec[dim_n-1] = (remainder/denom);
    size_t new_rem = remainder % denom;
    getSubIdx(vec, new_rem, helper<dim_n-1>());
}

template<size_t meshDim>
void Mesh<meshDim>::getSubIdx(std::vector<size_t> &vec, const size_t &remainder, helper<1>) const {
    vec[0] = (remainder);
}

// Instantiate Mesh templates
template class Mesh<1>;
template class Mesh<2>;
template class Mesh<3>;

template void Mesh<2>::getSubIdx(std::vector<size_t>&, const size_t&, helper<2>) const;
template void Mesh<3>::getSubIdx(std::vector<size_t>&, const size_t&, helper<3>) const;
