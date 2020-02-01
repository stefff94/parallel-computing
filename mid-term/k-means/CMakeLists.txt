cmake_minimum_required(VERSION 3.3)
project(k_means_mid_term)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp")

include_directories(/usr/local/include/)
link_directories(/usr/local/lib/)

add_executable("sequential" sequential.cpp utility.h)
add_executable("parallel" parallel.cpp utility.h)
add_executable("plot" plot.cpp gnuplot.h)