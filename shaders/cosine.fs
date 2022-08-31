#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 diffuse;
};

out vec4 color;

uniform Light light;

void main() {
    vec3 textureColor = vec3(0.9,0.9,0.9);
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0); //cos
    vec3 diffuse = light.diffuse * diff * textureColor;
    
    color = vec4(diffuse, 1.0f);
}