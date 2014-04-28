cmake_minimum_required (VERSION 2.8)
project (VoxelRenderer)

subdirs (src include lib)

find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)

add_executable (main src/main.cpp)
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
