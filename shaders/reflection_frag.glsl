#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 cameraPos;
uniform samplerCube skyboxCubemap; // Cubemap texture unit

void main() {
    vec3 I = normalize(FragPos - cameraPos);
    vec3 N = normalize(Normal);
    vec3 R = reflect(I, N);

    // Sample reflection from the environment cubemap
    vec3 reflectionColor = texture(skyboxCubemap, R).rgb;

    FragColor = vec4(reflectionColor, 1.0);
}