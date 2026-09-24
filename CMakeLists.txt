cmake_minimum_required(VERSION 3.30.0)

project(MyNewLang)

file(GLOB_RECURSE SNAP_SRC src/Snap/*.cpp)
file(GLOB_RECURSE TEST_SRC src/Test/*.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_NOTES ON)

include_directories(src/)
include_directories(src/Snap/)

add_library(Snap ${SNAP_SRC})
add_executable(Test ${TEST_SRC})

# configure_file(fib.txt fib.txt COPYONLY)
configure_file(test.snapir test.snapir COPYONLY)

target_link_libraries(Test PRIVATE Snap)