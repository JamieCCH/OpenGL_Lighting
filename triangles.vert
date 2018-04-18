#version 410 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec3 aNormal;

out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;

// Values that stay constant for the whole mesh.
uniform highp mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertex_position,1.0f);
    
    FragPos = vec3(mat4(1.0f) * vec4(vertex_position, 1.0));
    
    texCoord = vertex_texture;
    Normal = mat3(transpose(inverse(mat4(1.0f)))) * aNormal;

//    Normal = aNormal;
    
}
