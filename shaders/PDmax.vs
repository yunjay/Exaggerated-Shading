#version 430 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 maxPD;
layout (location = 4) in vec3 minPD;
layout (location = 5) in float maxCurv;
layout (location = 6) in float minCurv;

out VS_OUT {
    vec3 maxPrincipal;
    vec3 minPrincipal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
//uniform mat4 projection;

void main()
{
    gl_Position =  view * model * vec4(aPos, 1.0); 
    vs_out.maxPrincipal = mat3(transpose(inverse(view*model))) * maxPD;
    vs_out.minPrincipal = mat3(transpose(inverse(view*model))) * minPD;

}