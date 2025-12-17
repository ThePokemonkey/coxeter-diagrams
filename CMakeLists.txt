cmake_minimum_required(VERSION 3.12)
project(coxeter-diagrams VERSION 0.1.0 LANGUAGES CXX)

#require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(coxeter-diagrams 
    src\\main.cpp 
    src\\label.cpp
)

#PLEASE require C++20
target_compile_features(coxeter-diagrams PRIVATE cxx_std_20)