#ifndef DIMENSIONMAP_H
#define DIMENSIONMAP_H

#include <map>
#include <string>

namespace DimensionMap
{
    using mT = typename std::map<size_t, std::string>;
    using T = mT::value_type;
    extern mT map;
}

#endif // DIMENSIONMAP_H
