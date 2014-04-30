#pragma comment(lib, "glew32.lib")
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "../include/Renderer.hpp"

void draw();

using namespace std;

#define CX 16
#define CY 16
#define CZ 16

GLuint program;
GLint attribute_coord2d;
GLint uniform_mvp;

typedef glm::detail::tvec4<GLbyte> byte4;


/** Compiles our shaders and links them to our program **/
int init_resources()
{
  GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  const char *vs_source =
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"  // OpenGL ES 2.0
#else
    "#version 120\n"  // OpenGL 2.1
#endif
    "attribute vec2 coord2d;                  "
	"varying vec4 texcoord;                   "
	"uniform mat4 mvp;                        "
    "void main(void) {                        "
    "  gl_Position = vec4(coord2d, 0.0, 1.0); "
    "}";
  glShaderSource(vs, 1, &vs_source, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    fprintf(stderr, "Error in vertex shader\n");
    return 0;
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fs_source =
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"  // OpenGL ES 2.0
#else
    "#version 120\n"  // OpenGL 2.1
#endif
    "attribute vec4 texcoord  "
	"void main(void) {        "
    "   gl_FragColor = vec4(texcoord.w / 128.0, texcoord.w / 256.0, texcoord.w / 512.0, 1.0);"
    "}";
  glShaderSource(fs, 1, &fs_source, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    fprintf(stderr, "Error in fragment shader\n");
    return 0;
  }

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    return 0;
  }

  const char* attribute_name = "coord2d";
  attribute_coord2d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord2d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  return 1;
}

struct chunk {
  uint8_t blk[CX][CY][CZ];
  GLuint vbo;
  int elements;
  bool changed;
 
  chunk() {
    memset(blk, 0, sizeof blk);
    elements = 0;
    changed = true;
    glGenBuffers(1, &vbo);
  }
 
  ~chunk() {
    glDeleteBuffers(1, &vbo);
  }
 
  uint8_t get(int x, int y, int z) {
    return blk[x][y][z];
  }
 
  void set(int x, int y, int z, uint8_t type) {
    blk[x][y][z] = type;
    changed = true;
  }
 
  void update() {
    changed = false;
    // Fill in the VBO here
  }
 
  void render() {
    if(changed)
      update();
    // Render the VBO here
  }
};


int main(int argc, char* argv[])
{
	uint8_t lol;

    glutInit(&argc, argv);

    // Simple buffer
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50, 25);
    glutInitWindowSize(500, 250);
    glutCreateWindow("Green Window");

    glm::vec3 a; // Test

    // Draw
    glutDisplayFunc(draw);
    glutMainLoop();
    cout << "Hello" << endl;
    return 0;
}

void draw(void)
{
  // Background colour
  glClearColor(0, 1, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw order
  glFlush();
}
