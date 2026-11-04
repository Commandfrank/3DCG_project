#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 cameraPos;
uniform vec3 lightPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    // Fresnel effect: highly reflective at glancing angles
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
    fresnel = clamp(fresnel + 0.1, 0.0, 1.0);

    // High intensity specular highlight
    float spec = pow(max(dot(norm, halfDir), 0.0), 256.0);
    vec3 specular = vec3(1.0) * spec * 2.0;

    // Metallic mirror base tint blended with highlights
    vec3 mirrorBase = vec3(0.85, 0.88, 0.92);
    vec3 finalColor = mix(mirrorBase * 0.2, mirrorBase + specular, fresnel);

    FragColor = vec4(finalColor, 1.0);
}