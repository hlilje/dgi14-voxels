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

GLuint program;
GLint attribute_coord;
GLint uniform_mvp;

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

        byte4 vertex[CX * CY * CZ * 6 * 6];
        int i = 0;

        for(int x = 0; x < CX; x++)
        {
            for(int y = 0; y < CY; y++)
            {
                for(int z = 0; z < CZ; z++)
                {
                    // Empty block?
                    if(!blk[x][y][z])
                        continue;

                    // View from negative x
                    vertex[i++] = byte4(x,     y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);

                    // View from positive x
                    vertex[i++] = byte4(x + 1, y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y    , z + 1, blk[x][y][z]);

                    // View from negative y 
                    vertex[i++] = byte4(x,     y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);

                    // View from positive y 
                    vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, blk[x][y][z]);

                    // View from negative z
                    vertex[i++] = byte4(x,     y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z,     blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z,     blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z,     blk[x][y][z]);

                    // View from positive z
                    vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y,     z + 1, blk[x][y][z]);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, blk[x][y][z]);
                }
            }
        }

        elements = i;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
    }

    void render()
    {
        if(changed)
            update();

        // If this chunk is empty, we don't need to draw anything.
        if(!elements)
            return;

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements);
    }
};
