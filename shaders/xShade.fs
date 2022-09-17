#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 diffuse;
};

out vec4 color;
//"global" lighting
uniform Light light;
uniform float clampCoef;
uniform int scales;
uniform float contribution[20];

void main() {
    vec3 textureColor = vec3(0.9,0.9,0.9);
    // first normal
    vec3 norm = normalize(Normal);
    vec3 lightGlobal = normalize(light.position - FragPos);
    //c_i = a*clamp(n_i dot  light_ip1)
    //l_(i+1) = l_global - normal_i+1 * (normal_i+1 dot l_global)     
    vec3 light_i;
    vec3 light_ip1;
    vec3 c_i = clampCoef*clamp(dot(normal_i,light_ip1));
    for(int i=0;i<20;i++){

    }
    color = vec4(lighting, 1.0f);

}