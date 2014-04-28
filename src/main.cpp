#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "include/Renderer.hpp"

void draw();

using namespace std;

int main(int argc, char* argv[])
{
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
