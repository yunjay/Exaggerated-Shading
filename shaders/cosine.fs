#version 430

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 diffuse;
};

out vec4 color;

uniform vec3 textureColor;
uniform Light light;

void main() {
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0); //cos
    vec3 diffuse = light.diffuse * diff * textureColor;
    
    vec3 ambient = vec3(0.1f)*textureColor;
    color = vec4(ambient+diffuse, 1.0f);
}