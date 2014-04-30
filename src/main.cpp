#include "../include/main.hpp"

using namespace std;

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

    if(!compile_ok)
    {
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

    if(!compile_ok)
    {
        fprintf(stderr, "Error in fragment shader\n");
        return 0;
    }

    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &link_ok);

    if(!link_ok)
    {
        fprintf(stderr, "glLinkProgram:");
        return 0;
    }

    const char* attribute_name = "coord2d";
    _attribute_coord2d = glGetAttribLocation(_program, attribute_name);

    if(_attribute_coord2d == -1)
    {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }

    return 1;
}

void draw()
{
    // Background colour
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw order
    glFlush();
}

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
