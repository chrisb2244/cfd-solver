set(FieldOp_SRCS
    FieldOperations.cpp
    )

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/DataStructures)

add_library(fieldOperations SHARED ${FieldOp_SRCS})

target_link_libraries(fieldOperations dataStructures)
