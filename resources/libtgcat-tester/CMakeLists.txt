cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(tgcat-tester VERSION 1.0.0 LANGUAGES C)

add_executable(tgcat-tester tester.c json.c json-builder.c)

find_library(TGCAT_LIBRARY tgcat PATHS "${CMAKE_CURRENT_SOURCE_DIR}" NO_DEFAULT_PATH)
target_link_libraries(tgcat-tester PRIVATE "${TGCAT_LIBRARY}")

if (NOT WIN32)
  target_link_libraries(tgcat-tester PRIVATE m)
  if (UNIX AND NOT APPLE)
    target_link_libraries(tgcat-tester PRIVATE rt)
  endif()
else()
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
