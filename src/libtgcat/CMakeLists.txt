cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(tgcat VERSION 1.0.0)

set(CMAKE_CXX_FLAGS " -pthread -std=c++17 -funroll-loops -O3 -march=native")

message("${CMAKE_SOURCE_DIR}/../../resources/fasttext/")

add_library(fasttext SHARED IMPORTED)
set_target_properties(fasttext PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/../../resources/fasttext/src/"
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../resources/fasttext/build/libfasttext.so"
)

add_library(tgcat SHARED tgcat.cpp preprocessor.cpp language_predictor.cpp)
target_link_libraries(tgcat fasttext)
