#version 430 

in vec2 TexCoords;
//in vec3 col;
in float col;

out vec4 color;
//"global" lighting

uniform vec3 highColor;
uniform vec3 lowColor;


void main() {
    //color = vec4(col, 1.0f);
    color = vec4( col * (highColor-lowColor) + lowColor,1.0);
}