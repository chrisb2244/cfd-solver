set(FieldOp_SRCS
    FieldOperations.tpp
    )

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/DataStructures)

add_library(fieldOperations SHARED ${FieldOp_SRCS})
set_target_properties(fieldOperations PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(fieldOperations dataStructures)
