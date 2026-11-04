#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalModelMatrix;

out vec3 FragPos;
out vec3 Normal;

void main() {
    // World space position
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    
    // World space normal
    Normal = normalize(normalModelMatrix * aNormal);
    
    gl_Position = mvpMatrix * vec4(aPos, 1.0);
}