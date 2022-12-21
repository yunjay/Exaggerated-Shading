#version 430 

in vec2 TexCoords;
//in vec3 col;
in float col;

out vec4 color;
//"global" lighting

uniform vec3 textureColor;

void main() {
    //color = vec4(col, 1.0f);
    color=col*vec4(textureColor,1.0f);
}