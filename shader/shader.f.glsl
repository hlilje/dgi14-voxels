#version 120 // OpenGL 2.1

varying vec4 texcoord;
uniform sampler2D texture;

void main(void) {
    vec2 coord2d = vec2((fract(texcoord.x) + texcoord.w) / 16.0, texcoord.z);
    gl_FragColor = texture2D(texture, coord2d);
}
