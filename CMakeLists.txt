cmake_minimum_required(VERSION 3.9)
project(Parallel_Decision_Tree_Classifier_master)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "-fopenmp")

add_executable(Parallel_Decision_Tree_Classifier_master
        dtc_seq.cpp
        gputimer.h Node.cpp Node.h)
