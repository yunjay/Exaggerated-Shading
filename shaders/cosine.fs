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
    vec3 textureColor = vec3(0.8,0.8,0.8);
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0); //cos
    vec3 diffuse = light.diffuse * diff * textureColor;
    
    vec3 ambient = vec3(0.1f);
    color = vec4(ambient+diffuse, 1.0f);
}