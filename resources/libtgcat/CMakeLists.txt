cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(tgcat VERSION 1.0.0 LANGUAGES C)

add_library(tgcat SHARED tgcat.c)
