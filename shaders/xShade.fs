#version 430 

in vec2 TexCoords;
in vec3 col;


out vec4 color;
//"global" lighting

//uniform float contribution[20];

void main() {
    color = vec4(col, 1.0f);
}