cmake_minimum_required(VERSION 3.24)
project(mpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(mpp main.cpp metamodel.h)
add_executable(sample1 samples/sample1/main.cpp samples/sample1/main.cpp metamodel.h samples/sample1/sample_meta_model.cpp samples/sample1/sample_meta_model.h)