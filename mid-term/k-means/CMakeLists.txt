cmake_minimum_required(VERSION 3.3)
project(k_means_mid_term)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp")

include_directories(/usr/local/include/)
link_directories(/usr/local/lib/)

add_executable("k-means-sequential" sequential.cpp utility.h)
add_executable("k-means-parallel-array-of-structures" parallel-array-of-structures.cpp utility.h)
add_executable("k-means-parallel-structure-of-arrays" parallel-structure-of-arrays.cpp utility-v2.h)
add_executable("plot" plot.cpp gnuplot.h)