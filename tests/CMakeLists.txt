set(TEST_SRCS
    testFields.cpp
    )

include_directories(
    ${CMAKE_SOURCE_DIR}/DataStructures
    ${CMAKE_SOURCE_DIR}/FieldOperations
    )

add_executable(testMain buildCatch.cpp
    ${TEST_SRCS})

target_link_libraries(testMain
    dataStructures
    fieldOperations
    )
