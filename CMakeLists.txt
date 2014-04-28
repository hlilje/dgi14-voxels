cmake_minimum_required (VERSION 2.8)
project (VoxelRenderer)

subdirs (src include)

find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)

set (SOURCES src/main.cpp src/Renderer.cpp)
set (HEADERS include/Renderer.hpp)

if (CMAKE_COMPILER_IS_GNUCXX)
    message (STATUS "GCC detected, adding compile flags")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Werror")
    set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
else (include_directories (${PROJECT_SOURCE_DIR}/glm))
endif (CMAKE_COMPILER_IS_GNUCXX)

add_executable (main ${SOURCES} ${HEADERS})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
