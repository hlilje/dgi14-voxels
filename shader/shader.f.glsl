#version 120 // OpenGL 2.1

varying vec4 texcoord;
uniform sampler2D texture;

void main(void) {
    gl_FragColor = vec4(texcoord.x / 100.0, texcoord.y / 100.0, texcoord.z / 200.0, 1.0);
}
