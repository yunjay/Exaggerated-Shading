#version 430 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 maxPD;
layout (location = 4) in vec3 minPD;
layout (location = 5) in float maxCurv;
layout (location = 6) in float minCurv;
//vec3 smoothedNormals[20][]; -> not legal

layout(binding = 7, std430) buffer smoothedNormalsBuffer  
{
    vec4 smoothedNormals[];
};

struct Light {
    vec3 position;
    vec3 diffuse;
};
out vec2 TexCoords;
out float col;

//out vec3 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//"global" lighting
uniform Light light;
uniform float ambient;
uniform float pdScaler;

void main() {
    gl_Position = projection * view *  model * vec4(aPos, 1.0f);
    vec3 FragPos = vec3(model * vec4(aPos, 1.0f)); 
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 lightGlobal = normalize(light.position - FragPos);
    vec3 lightEffective = pdScaler*(maxCurv*maxCurv - minCurv*minCurv)*dot(maxPD,lightGlobal)*maxPD + lightGlobal;
    lightEffective = normalize(lightEffective);
    col = ambient + dot(lightEffective,Normal);
}
