project(CFD-Solver-October)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
    main.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

# General flags for all build types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")
# Flags for debug build only
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wunused -Wextra -Wno-deprecated-declarations")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(DataStructures)
add_subdirectory(FieldOperations)
add_subdirectory(tests)
target_link_libraries(${PROJECT_NAME} dataStructures fieldOperations)
