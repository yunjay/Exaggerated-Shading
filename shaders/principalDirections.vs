#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position =  view * model * vec4(aPos, 1.0); 
    vs_out.normal = vec3(vec4(mat3(transpose(inverse(view*model))) * aNormal, 0.0));
}