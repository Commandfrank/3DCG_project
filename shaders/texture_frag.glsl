#version 410

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
    vec3 ks;
    float shininess;
    float transparency;
};

uniform vec3 sunPos;
uniform vec3 cameraPos;
uniform vec3 sunColor;
uniform sampler2D colorMap;
uniform bool hasTexCoords;
uniform bool useMaterial;

uniform vec4 baseColor;
uniform bool useBaseColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(1.0, 1.5, 0.5));
    float diff = max(dot(normal, lightDir), 0.4);

    if (hasTexCoords) {
        vec4 tex = texture(colorMap, fragTexCoord);
        fragColor = vec4(tex.rgb * diff, tex.a);
    }
    else if (useMaterial)   { fragColor = vec4(kd * diff, 1.0);}
    else                    { fragColor = vec4(normal, 1); } // Output color value, change from (1, 0, 0) to something else
}
