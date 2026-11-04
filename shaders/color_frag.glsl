#version 410

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
    vec3 ks;
    float shininess;
    float transparency;
};

uniform vec4 baseColor;
uniform vec3 sunPos;
uniform vec3 sunColor;
uniform vec3 cameraPos;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{

    vec3 surfaceColor = baseColor.rgb;

    // Lightning with Blinn-Phong
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPos - fragPosition);
    vec3 L = normalize(sunPos - fragPosition);
    vec3 H = normalize(V + L);

    vec3 ambient = 0.25 * surfaceColor * sunColor;

    float NL = max(dot(N, L), 0.0);
    vec3 diffuse = NL * surfaceColor * sunColor;

//    shininess = 32.0;
//    ks = 0.5;
    float area = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = ks * area * sunColor;

    fragColor = vec4(ambient + diffuse + specular, baseColor.a);
}