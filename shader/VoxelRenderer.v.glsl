#version 120
 
varying vec4 texcoord;
 
void main(void) {
    //Placeholder color, 4th dimension is alpha
    gl_FragColor = vec4(texcoord.w / 128.0, texcoord.w / 256.0, texcoord.w / 512.0, 1.0);
}