#version 410

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
    vec3 ks;
    float shininess;
    float transparency;
};

uniform vec3 sunPos;
uniform vec3 sunColor;
uniform vec3 cameraPos;


uniform bool isMetal;
uniform float roughness;

uniform bool enableSun;
uniform bool enableInteriorLights;
uniform vec3 pointLightPositions[1];
uniform vec3 pointLightColors[1];
uniform vec3 spotlightDirections[1];

uniform sampler2D texShadow;
uniform mat4 lightMVP;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

const float PI = 3.1415926536;

float ShadowCalculation(vec3 worldPos, vec3 N, vec3 L){
    if (sunPos.y <= 0.0){
        return 0.0;
    }

    vec4 fragLightCoord = lightMVP * vec4(worldPos, 1.0);
    vec3 projCoords = fragLightCoord.xyz / fragLightCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0){
        return 0.0;
    }
    float closestDepth = texture(texShadow, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(N,L)), 0.0005);

    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

vec3 CalculatePBRLight(vec3 L, vec3 V, vec3 N, vec3 intensity, vec3 surfaceColor){
    vec3 H = normalize(V + L);
    float NL = max(dot(N, L), 0.00001);
    float HN = max(dot(H, N), 0.00001);
    float NV = max(dot(N, V), 0.00001);
    float VH = max(dot(V, H), 0.00001);

    vec3 flambert = vec3(0.0);
    if(!isMetal){
        flambert = surfaceColor;
    }

    float D = (pow(roughness, 2.0)) / (PI * pow((pow(NL,2.0) * ((pow(roughness, 2.0) -1.0) + 1.0)),2.0));


    float K = pow(roughness + 1.0, 2.0) / 8.0;
    float G_V = NV / (NV * (1.0-K) + K);
    float G_L = NL / (NL * (1.0-K) + K);
    float G = G_V * G_L;

    vec3 F0 = vec3(0.04);
    if(isMetal){
        F0 = surfaceColor;
    }
    vec3 F = F0 + (vec3(1.0)-F0) * pow(clamp(1.0-VH,0.0,1.0),5.0);

    vec3 computedKs = F;
    vec3 computedKd = vec3(1.0) - computedKs;


    if(isMetal){
        computedKd = vec3(0.0);
    }

    vec3 fCookTorrance = (D * G * F) / max((4.0 * NL * NV + 0.0001), 0.0001);

    vec3 diffuseBRDF = (computedKd * flambert) / PI;
    vec3 specularBDRF = computedKs * fCookTorrance;


    return (diffuseBRDF + specularBDRF) * intensity * NL;
}
void main()
{
    // procedural Brick Generation
    // scale the UVs to create small repeating bricks
    vec2 uv = fragTexCoord * vec2(20.0, 10.0);

    // offset every other row for a classic brick pattern
    if (fract(uv.y * 0.5) > 0.5) {
        uv.x += 0.5;
    }
    vec2 grid = fract(uv);
    float isMortar = step(grid.x, 0.05) + step(grid.y, 0.05);
    isMortar = clamp(isMortar, 0.0, 1.0);

    vec3 brickColor = vec3(0.7, 0.25, 0.2);
    vec3 mortarColor = vec3(0.8, 0.8, 0.8);
    vec3 wall = mix(brickColor, mortarColor, isMortar);

    // apply lighting
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPos - fragPosition);
    vec3 totalLighting = vec3(0.0);

    if (enableSun) {
        vec3 L = normalize(sunPos);
        float shadow = ShadowCalculation(fragPosition, N, L);
        float sunIntensity = smoothstep(0.0, 2.0, sunPos.y);

        if (sunPos.y > 0.0) {
            vec3 sunRadiance = sunColor * sunIntensity;
            totalLighting += CalculatePBRLight(L, V, N, sunRadiance, wall) * (1.0 - shadow);
        }
    }
    if (enableInteriorLights) {
        float pointLightIntensity = 12.0;
        for (int i = 0; i < 1; ++i) {
            vec3 lightDelta = pointLightPositions[i] - fragPosition;
            float distance = length(lightDelta);
            vec3 L = normalize(lightDelta);

            float attenuation = 1.0 / ((distance * distance) + 0.5);
            float theta = dot(normalize(spotlightDirections[i]), -L);

            float innerCutoff = 0.866f;
            float outerCutoff = 0.707f;

            float epsilon = innerCutoff - outerCutoff;
            float intensityFactor = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

            vec3 radiance = pointLightColors[i] * pointLightIntensity * attenuation * intensityFactor;

            totalLighting += CalculatePBRLight(L, V, N, radiance, wall);
        }
    }

    // to make sure shadows arent pitch black
    float timeOfDay = smoothstep(-2.0, 2.0, sunPos.y);
    vec3 dayAmbient = vec3(0.05) * wall * sunColor;
    vec3 nightAmbient = vec3(0.02) * wall * vec3(0.2, 0.3, 0.5);

    vec3 ambient = mix(nightAmbient, dayAmbient, timeOfDay);

    if (!enableSun) {
        ambient = vec3(0.05) * wall;
    }

    totalLighting += ambient;

    totalLighting = totalLighting / (totalLighting + vec3(1.0));
    totalLighting = pow(totalLighting, vec3(1.0/2.2));
    fragColor = vec4(totalLighting, 1.0);
}
