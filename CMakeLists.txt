cmake_minimum_required (VERSION 2.6)
project (VoxelRenderer)

subdirs (src include)

add_executable (main src/main.cpp)
