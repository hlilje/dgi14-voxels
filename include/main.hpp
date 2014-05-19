#pragma once
//#pragma (lib, "glew32.lib");

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/PerlinNoise.hpp"

//#include "../src/perlin.c" // Bad

#define CX 32
#define CY 32
#define CZ 32

#define SCX 64
#define SCY 64
#define SCZ 64
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PROGRAM_NAME "Voxel Renderer"

// Use medium precision
typedef glm::detail::tvec4<GLbyte, glm::mediump> byte4;

GLuint program;
GLint attribute_coord;
GLint uniform_mvp;

glm::vec3 cameraPos(25, 25, 25);
glm::vec3 cameraLook = glm::normalize(glm::vec3(0, 0, 0) - cameraPos);
glm::mat4 mvp;

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

    void update()
    {
        changed = false;

        byte4 vertex[CX * CY * CZ * 6 * 6];
        int i = 0;

        for(int x = 0; x < CX; x++)
        {
            for(int y = 0; y < CY; y++)
            {
                bool visible = false;
                for(int z = 0; z < CZ; z++)
                {
                    if(!blk[x][y][z]) // Empty block?
                    {
                        visible = false;
                        continue;
                    }

                    // View from negative x

                    // Check if we are the same type as the previous block,
                    // if so merge the triangles
                    if(visible && blk[x][y][z] == blk[x - 1][y][z])
                    {
                        vertex[i - 5] = byte4(x,     y,     z + 1, blk[x][y][z]);        
                        vertex[i - 2] = byte4(x,     y,     z + 1, blk[x][y][z]);        
                        vertex[i - 1] = byte4(x,     y + 1, z + 1, blk[x][y][z]);        
                    }

                    else

                    // Only draw if no block in front
                    if(x > 0 && !blk[x - 1][y][z])
                    {
                        vertex[i++] = byte4(x,     y,     z,     blk[x][y][z]);
                        vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                        vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                        vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                        visible = true;
                    }
                    else
                    {
                        visible = false;
                    }

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
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Define array of generic vertex attribute data
        glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements); // Render primitives from array data
    }
};

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

    void render()
    {
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++)
                    if(c[x][y][z])
                    {
                        //glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(x * CX, y * CY, z * CZ));
                        // Calculate the full MVP matrix here and pass it to the vertex shader
                        c[x][y][z]->render();
                    }
    }
};
