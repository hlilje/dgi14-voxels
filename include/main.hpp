#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <cstring>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/noise.hpp"
#include "../shader/textures.c"

// Defined constants
#define CX 16
#define CY 16
#define CZ 16
#define SCX 25
#define SCY 25
#define SCZ 25
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PROGRAM_NAME "Voxel Renderer"

// Use medium precision
typedef glm::detail::tvec4<GLbyte, glm::mediump> byte4;

// Global OpenGL variables
GLuint program; // The program id
GLuint window_id; // Holds the id for the render window
GLint attribute_coord;
GLint uniform_mvp;
GLuint texture;
GLint uniform_texture;
GLuint cursor_vbo;
GLuint sun_vbo;

// Global matrices and vectors
glm::vec3 camera_pos(200.0, 100.0, 200.0);
glm::vec3 camera_look = glm::normalize(glm::vec3(-20.0, -50.0, -20.0));
glm::mat4 projection;
glm::mat4 view;
glm::mat4 mvp;
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 model = glm::mat4(1.0f); // Changes for each model

// Global integer variables
int ww, wh; // Window size
int cx, cy, cz; // Voxel currently looked at
int nx, ny, nz; // Voxel to be created

void update_mvp();

// Data structure to hold a set of voxels
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
        // Set array where to store generated buffer object names
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

    void unset(int x, int y, int z)
    {
        blk[x][y][z] = 0;
        changed = true;
    }

    void update()
    {
        changed = false;

        byte4 vertex[CX * CY * CZ * 6 * 6];
        int i = 0;

        int texture = 0;

        //Here we add the vertices for the faces of our voxels to the vertex array
        for(int x = 0; x < CX; x++)
        {
            for(int z = 0; z < CZ; z++)
            {
                for(int y = CY - 1; y >= 0; y--)
                {
                    if(!blk[x][y][z]) // Empty block?
                    {
                        continue;
                    }

                    // View from negative x
                    // Only draw if no block in front
                    if(x == 0 || !blk[x - 1][y][z])
                    {
                        //If there's a block on top, we place the earth texture
                        //Otherwise earth with grass side texture
                        if(y == (CY - 1) || !blk[x][y + 1][z])
                        {
                            texture = 2;
                        }
                        else
                        {
                            texture = 4;
                        }

                        vertex[i++] = byte4(x,     y,     z,     texture);
                        vertex[i++] = byte4(x,     y,     z + 1, texture);
                        vertex[i++] = byte4(x,     y + 1, z,     texture);
                        vertex[i++] = byte4(x,     y + 1, z,     texture);
                        vertex[i++] = byte4(x,     y,     z + 1, texture);
                        vertex[i++] = byte4(x,     y + 1, z + 1, texture);

                    }

                    // View from positive x
                    if(x == (CX - 1) || !blk[x + 1][y][z])
                    {
                        if(y == (CY - 1) || !blk[x][y + 1][z])
                        {
                            texture = 2;
                        }
                        else
                        {
                            texture = 4;
                        }

                        vertex[i++] = byte4(x + 1, y,     z,     texture);
                        vertex[i++] = byte4(x + 1, y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y,     z + 1, texture);
                        vertex[i++] = byte4(x + 1, y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y + 1, z + 1, texture);
                        vertex[i++] = byte4(x + 1, y    , z + 1, texture);
                    }

                    // View from negative y
                    if(y == 0 || !blk[x][y - 1][z])
                    {
                        //Earth texture
                        texture = 1;

                        vertex[i++] = byte4(x,     y,     z,     texture);
                        vertex[i++] = byte4(x + 1, y,     z,     texture);
                        vertex[i++] = byte4(x,     y,     z + 1, texture);
                        vertex[i++] = byte4(x + 1, y,     z,     texture);
                        vertex[i++] = byte4(x + 1, y,     z + 1, texture);
                        vertex[i++] = byte4(x,     y,     z + 1, texture);
                    }

                    // View from positive y
                    if(y == (CY - 1) || !blk[x][y + 1][z])
                    {
                        //Grass texture
                        texture = 3;

                        vertex[i++] = byte4(x,     y + 1, z,     texture);
                        vertex[i++] = byte4(x,     y + 1, z + 1, texture);
                        vertex[i++] = byte4(x + 1, y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y + 1, z,     texture);
                        vertex[i++] = byte4(x,     y + 1, z + 1, texture);
                        vertex[i++] = byte4(x + 1, y + 1, z + 1, texture);
                    }

                    // View from negative z
                    if(z == 0 || !blk[x][y][z - 1])
                    {
                        if(y == (CY - 1) || !blk[x][y + 1][z])
                        {
                            texture = 2;
                        }
                        else
                        {
                            texture = 4;
                        }

                        vertex[i++] = byte4(x,     y,     z,     texture);
                        vertex[i++] = byte4(x,     y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y,     z,     texture);
                        vertex[i++] = byte4(x,     y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y + 1, z,     texture);
                        vertex[i++] = byte4(x + 1, y,     z,     texture);
                    }

                    // View from positive z
                    if(z == (CZ - 1) || !blk[x][y][z + 1])
                    {
                        if(y == (CY - 1) || !blk[x][y + 1][z])
                        {
                            texture = 2;
                        }
                        else
                        {
                            texture = 4;
                        }

                        vertex[i++] = byte4(x,     y,     z + 1, texture);
                        vertex[i++] = byte4(x + 1, y,     z + 1, texture);
                        vertex[i++] = byte4(x,     y + 1, z + 1, texture);
                        vertex[i++] = byte4(x,     y + 1, z + 1, texture);
                        vertex[i++] = byte4(x + 1, y,     z + 1, texture);
                        vertex[i++] = byte4(x + 1, y + 1, z + 1, texture);
                    }
                }
            }
        }

        elements = i;
        glBindBuffer(GL_ARRAY_BUFFER, vbo); // Set target buffer object
        // Create and init buffer object's data store
        glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
    }

    void render()
    {
        if(changed)
            update();

        // Don't draw anything if the chunk is empty
        if(!elements)
            return;


        glEnable(GL_CULL_FACE); // Cull polys based on winding in window coord
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Define array of generic vertex attribute data
        glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements); // Render primitives from array data
    }
};

// Data structure to hold a set of chunks (makes up the world)
struct superchunk
{
    chunk *c[SCX][SCY][SCZ];

    superchunk()
    {
        memset(c, 0, sizeof c);
    }

    ~superchunk()
    {
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCX; y++)
                for(int z = 0; z < SCX; z++)
                    delete c[x][y][z];
    }

    uint8_t get(int x, int y, int z)
    {
        int cx = x / CX;
        int cy = y / CY;
        int cz = z / CZ;

        x %= CX;
        y %= CY;
        z %= CZ;

        if(!c[cx][cy][cz])
            return 0;
        else
            return c[cx][cy][cz]->get(x, y, z);
    }

    void set(int x, int y, int z, uint8_t type)
    {
        int cx = x / CX;
        int cy = y / CY;
        int cz = z / CZ;

        x %= CX;
        y %= CY;
        z %= CZ;

        if(!c[cx][cy][cz])
            c[cx][cy][cz] = new chunk();

        c[cx][cy][cz]->set(x, y, z, type);
    }

    void unset(int x, int y, int z)
    {
        int cx = x / CX;
        int cy = y / CY;
        int cz = z / CZ;

        x %= CX;
        y %= CY;
        z %= CZ;

        if(c[cx][cy][cz])
            c[cx][cy][cz]->unset(x, y, z);
    }

    void render()
    {
        for(int x = 0; x < SCX; x++)
        {
            for(int y = 0; y < SCY; y++)
            {
                for(int z = 0; z < SCZ; z++)
                {
                    if(c[x][y][z])
                    {
                        model = glm::translate(glm::mat4(1.0f), glm::vec3(x * CX, y * CY, z * CZ));
                        update_mvp();

                        // Is this chunk on the screen?
                        glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

                        center.x /= center.w;
                        center.y /= center.w;

                        // If it is behind the camera, don't bother drawing it
                        if(center.z < -CY / 2)
                            continue;

                        // If it is outside the screen, don't bother drawing it
                        if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
                            continue;

                        glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
                        // Calculate the full MVP matrix here and pass it to the vertex shader
                        c[x][y][z]->render();
                    }
                }
            }
        }
    }
};

superchunk world; // The container for all the world's voxels
