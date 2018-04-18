#version 410 core

out vec4 frag_colour;

in vec3 FragPos;
in vec2 texCoord;
in vec3 Normal;

uniform sampler2D texture0;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
//uniform vec3 objectColor;

void main() {
    
    // ambient
    float ambientStrength = 0.025;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.05;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = ambient + diffuse + specular;
    frag_colour = vec4(result, 1.0) * texture(texture0, texCoord);

}

