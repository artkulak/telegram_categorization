cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(tgcat VERSION 1.0.0)

set(CMAKE_CXX_FLAGS " -pthread -std=c++17 -funroll-loops -O3 -march=native")

include_directories(${CMAKE_SOURCE_DIR}/fasttext/src/)
file(GLOB FT_SRCS ${CMAKE_SOURCE_DIR}/fasttext/src/*.cc)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../)
add_library(tgcat SHARED tgcat.cpp preprocessor.cpp predictor.cpp ${FT_SRCS})
target_link_libraries(tgcat)
