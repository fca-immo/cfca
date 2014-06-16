cmake_minimum_required (VERSION 2.6)

project( fca )

SET(CMAKE_C_FLAGS_DEBUG "-O2 -Wall -std=c9x -g")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -std=c9x -g")

add_subdirectory(src)

