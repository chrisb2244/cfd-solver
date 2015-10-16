#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <iostream>
#include <cstddef>
#include <vector>

#include "DimensionMap.h"

template<size_t mD>
class BoundingBox
{
public:
    BoundingBox(std::vector<double>&& bounds):
        bounds_(bounds) {}

    const std::vector<double>& bounds() const { return bounds_; }

private:
    std::vector<double> bounds_;
};

template<size_t mD>
std::ostream& operator<<(std::ostream& os, const BoundingBox<mD>& box) {
    const std::vector<double>& bounds = box.bounds();
    auto map = DimensionMap::map;
    for (size_t d=0; d<mD-1; d++) {
        os << map[d] << ": [" << bounds[2*d] << ", " << bounds[(2*d)+1] << "], ";
    }
    os << map[mD-1] << ": [" << bounds[2*(mD-1)]
                    << ", " << bounds[(2*(mD-1))+1] << "]";
    return os;
}

#endif // BOUNDINGBOX_H
