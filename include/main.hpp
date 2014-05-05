#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>

#define CX 16
#define CY 16
#define CZ 16

// Use medium precision
typedef glm::detail::tvec4<GLbyte, glm::mediump> byte4;

// Compiles shaders and links it to the program
int init_resources();
void draw(void);
int main(int argc, char* argv[]);

GLuint _program;
GLint _attribute_coord2d;
GLint _uniform_mvp;

struct chunk
{
    uint8_t blk[CX][CY][CZ];
    GLuint vbo;
    int elements;
    bool changed;

    chunk()
    {
        memset(blk, 0, sizeof blk);
        elements = 0;
        changed = true;
        glGenBuffers(1, &vbo);
    }

    ~chunk()
    {
        glDeleteBuffers(1, &vbo);
    }

    uint8_t get(int x, int y, int z)
    {
        return blk[x][y][z];
    }

    void set(int x, int y, int z, uint8_t type)
    {
        blk[x][y][z] = type;
        changed = true;
    }

    void update()
    {
        changed = false;
        // Fill in the VBO here
    }

    void render()
    {
        if(changed)
            update();
        // Render the VBO here
    }
};
