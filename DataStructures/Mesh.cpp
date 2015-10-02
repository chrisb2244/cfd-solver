#include "Mesh.h"

#include <cmath>

template<size_t meshDim>
void Mesh<meshDim>::placeCentres(const int d) {
    position_[d].resize(dimSize_[d]);
    const double range = dimMax_[d] - dimMin_[d];
    switch(scalingType_)
    {
        // This is defaulted by the constructor
        case MeshScalingType::Constant:
        {
            // Constant spacing
            const double dx = range / dimSize_[d];
            for (size_t i=0; i<dimSize_[d]; i++) {
                position_[d][i] = (dimMin_[d] + (dx*(i+0.5)));
            }
        }
        break;

        case MeshScalingType::Exponential:
        {
            // Exponential spacing
            constexpr double b = 3;
            constexpr double dom = exp(b)-1;
            for (size_t i=0; i<dimSize_[d]; i++) {
                const double u = static_cast<double>(i+0.5) / dimSize_[d];
                const double x = ((exp(b*u) - 1)/dom);
                position_[d][i] = (x*range) + dimMin_[d];
            }
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
                const double u = static_cast<double>(i+0.5) / dimSize_[d];
                const double x = (tanh(b*(u-c)) - A) / (B-A);
                position_[d][i] = (x*range) + dimMin_[d];
            }
        }
        break;

        case MeshScalingType::Pivot:
        {
            // Pivoted spacing - requires an even number of cells
            assert(dimSize_[d]%2==0);
            constexpr double p=0.35;
            constexpr double b = 2;
            constexpr double dom = exp(b)-1;
            const double mid = (range * p) + dimMin_[d];
            const size_t dim2 = dimSize_[d]/2;
            for (size_t i=0; i<dim2; i++) {
                const double u = static_cast<double>(i+0.5) / dim2;
                const double x = ((exp(b*u) - 1)/dom);
                const double step_lower = x*range*(p < 0.5 ? p : 1-p);
                const double step_upper = x*range*(p < 0.5 ? 1-p : p);
                // Upper half
                position_[d][i+dim2] = mid + step_upper;
                // Lower half
                position_[d][dim2-i-1] = mid - step_lower;
            }
        }
        break;
    }
}

template<size_t meshDim>
double Mesh<meshDim>::getPosition(const size_t d, const int &i) const
{
    // Bounds checking if NDEBUG not defined
    assert(i>=0);
    assert(i<dimSize_[d]);
    return position_[d][i];
}

template<size_t meshDim>
void Mesh<meshDim>::checkBounds(std::vector<size_t> idxs) const
{
    for (size_t d=0; d<meshDim; d++) {
        assert(idxs[d] < dimSize_[d]);
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

// Instantiate Mesh templates
template class Mesh<1>;
template class Mesh<2>;
template class Mesh<3>;
