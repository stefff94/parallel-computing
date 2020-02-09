cmake_minimum_required(VERSION 3.15)
project(ngrams_final_term_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(ngrams_cpp_parallel main.cpp Producer.h Consumer.h MyQueue.h MyMap.h)

add_executable(ngrams_cpp_sequential sequential.cpp)