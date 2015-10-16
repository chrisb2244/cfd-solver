set(DataSRCS
    Vector.tpp
    Mesh.cpp
    Field.tpp
    BoundingBox.cpp
    DimensionMap.cpp
    )

include_directories(${CMAKE_SOURCE_DIR})
add_library(dataStructures SHARED ${DataSRCS})
