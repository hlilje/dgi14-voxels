//#pragma (lib, "glew32.lib");
#pragma once

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

#define CX 32
#define CY 32
#define CZ 32
#define SCX 32
#define SCY 32
#define SCZ 32
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PROGRAM_NAME "Voxel Renderer"

// Use medium precision
typedef glm::detail::tvec4<GLbyte, glm::mediump> byte4;

GLuint program;
GLint attribute_coord;
GLint uniform_mvp;

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
// Camera matrix
glm::mat4 View       = glm::lookAt(
    glm::vec3(55, 55, 55), // The position which the camera has in world space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0) // Head is up (set to 0,-1,0 to look upside-down)
);
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model      = glm::mat4(1.0f);  // Changes for each model
// ModelViewProjection : multiplication of our 3 matrices
// Matrix multiplication is the other way around
glm::mat4 mvp        = Projection * View * Model;

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
