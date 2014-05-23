#version 120 // OpenGL 2.1

varying vec4 texcoord;
uniform sampler2D texture;

const vec4 fogcolor = vec4(0.6, 0.8, 1.0, 1.0);
const float fogdensity = .00002;

void main(void) {
    
    vec4 color;

    //We get the index of the texture as the w coordinate
    //1 is earth, 2 is earth with grass on top, 3 is grass

    //Top and bottom textures
    if(texcoord.w == 1 || texcoord.w == 3)
        color = texture2D(texture, vec2(((fract(texcoord.x) + texcoord.w) / 16.0), texcoord.z));

    //Side textures
    else if(texcoord.w == 2)
        color = texture2D(texture, vec2((fract(texcoord.x + texcoord.z) + 2) / 16.0, -texcoord.y)) * 0.75;

    //Earth side texture
    else if(texcoord.w == 4)
        color = texture2D(texture, vec2((fract(texcoord.x + texcoord.z) + 1) / 16.0, -texcoord.y)) * 0.75;

    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fog = clamp(exp(-fogdensity * z * z), 0.2, 1);
 
    gl_FragColor = mix(fogcolor, color, fog);
}
