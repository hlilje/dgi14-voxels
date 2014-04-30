#pragma once

#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp>

// This class currently does nothing
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
