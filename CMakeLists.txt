cmake_minimum_required(VERSION 4.2 FATAL_ERROR)

project(Sample VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/main.cmake)

add_custom_target(programs)
add_custom_target(tests)

add_subdirectory(skarnc)
add_subdirectory(skarn)
