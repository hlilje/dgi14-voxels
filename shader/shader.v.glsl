#version 120 // OpenGL 2.1

attribute vec4 coord;
varying vec4 texcoord;
uniform mat4 Model;
uniform mat4 mvp;

void main(void) {
    texcoord = Model * coord;
    gl_Position = mvp * vec4(coord.xyz, 1);
}
