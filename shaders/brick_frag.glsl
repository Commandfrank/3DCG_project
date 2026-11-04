#version 410

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
    vec3 ks;
    float shininess;
    float transparency;
};

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 lightColor;
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
    // procedural Brick Generation
    // scale the UVs to create small repeating bricks
    vec2 uv = fragTexCoord * vec2(20.0, 10.0);

    // offset every other row for a classic brick pattern
    if (fract(uv.y * 0.5) > 0.5) {
        uv.x += 0.5;
    }

    // create the grid
    vec2 grid = fract(uv);

    // mortar thickness (0.05 is 5% of the cell)
    float isMortar = step(grid.x, 0.05) + step(grid.y, 0.05);
    isMortar = clamp(isMortar, 0.0, 1.0);

    vec3 brickColor = vec3(0.7, 0.25, 0.2);
    vec3 mortarColor = vec3(0.8, 0.8, 0.8);
    vec3 wall = mix(brickColor, mortarColor, isMortar);

    // apply lighting
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(1.0, 1.5, 0.5));
    float diff = max(dot(normal, lightDir), 0.4);

    fragColor = vec4(wall * diff, 1.0);
}
