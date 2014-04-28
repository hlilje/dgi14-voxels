#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp>
#pragma once

class Renderer
{
    public:
        Renderer(void);
        ~Renderer(void);

        void init(void);
    private:
        void draw(void);
        void update(void);
};
