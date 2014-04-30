#include "../include/Renderer.hpp"

void init(void);
void draw(void);
void update(void);

Renderer::Renderer()
{
}

void Renderer::draw()
{
  // Background colour
  glClearColor(0, 1, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw order
  glFlush();
}

void Renderer::update()
{
}
