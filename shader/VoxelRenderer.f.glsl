#version 120
 
attribute vec4 coord;
uniform mat4 mvp;
varying vec4 texcoord;
 
void main(void) {
    texcoord = coord;
    //4th dimension is for clipping
    gl_Position = mvp * vec4(coord.xyz, 1);
}