#include "DimensionMap.h"

namespace DimensionMap {
    mT map = []
    {
        using T = DimensionMap::T;
        DimensionMap::mT map;
        map.insert(T(0, "X"));
        map.insert(T(1, "Y"));
        map.insert(T(2, "Z"));
        return map;
    }();
}
