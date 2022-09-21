#version 430 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(binding = 3, std430) buffer smoothedNormalsBuffer  
{
	//vec3 smoothedNormals[20][]; -> not legal
    vec3 smoothedNormals[][20];  
};

out vec2 TexCoords;
out vec3 col;

struct Light {
    vec3 position;
    vec3 diffuse;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//"global" lighting

uniform Light light;

uniform float clampCoef;
uniform int scales;
uniform float contribution[20];

void main() {
    gl_Position = projection * view *  model * vec4(aPos, 1.0f);
    vec3 FragPos = vec3(model * vec4(aPos, 1.0f)); 
    //vec3 Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 Normal = normalize(aNormal);
    vec3 lightGlobal = normalize(light.position - FragPos);
    
    TexCoords = aTexCoords;
    vec3 textureColor = vec3(0.95,0.95,0.95);

    vec3 light_ip1;
    vec3 normal_i;
    vec3 normal_ip1;
    float detailTerms=0.0;
    float c_i=0.0;
    for(int i=1;i<scales;i++){
        //load smoothed normals
        normal_i=smoothedNormals[gl_VertexID][i-1];
        normal_ip1=smoothedNormals[gl_VertexID][i];
        
        light_ip1=normalize(lightGlobal-dot(lightGlobal,normal_ip1)*normal_ip1);
        c_i = clamp(clampCoef*dot(normal_i,light_ip1),-1.0,1.0);
        detailTerms+=contribution[i]*c_i;
    }
    col=(0.5 + 0.5*(contribution[0]*dot(Normal,lightGlobal)+detailTerms))*textureColor;

}
