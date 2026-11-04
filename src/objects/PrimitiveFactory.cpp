#include "PrimitiveFactory.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
DISABLE_WARNINGS_POP()

Mesh PrimitiveFactory::createCube() {
    Mesh mesh;
    // front z
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f,  0.5f}, .normal = {0.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f,  0.5f}, .normal = {0.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f,  0.5f}, .normal = {0.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f,  0.5f}, .normal = {0.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f} });
    //back -z
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f, -0.5f}, .normal = {0.0f, 0.0f, -1.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f, -0.5f}, .normal = {0.0f, 0.0f, -1.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f, -0.5f}, .normal = {0.0f, 0.0f, -1.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f, -0.5f}, .normal = {0.0f, 0.0f, -1.0f}, .texCoord = {0.0f, 1.0f} });
    //top y
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f, -0.5f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f, -0.5f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f,  0.5f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f,  0.5f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoord = {0.0f, 1.0f} });
    //bottom -y
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f,  0.5f}, .normal = {0.0f, -1.0f, 0.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f,  0.5f}, .normal = {0.0f, -1.0f, 0.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f, -0.5f}, .normal = {0.0f, -1.0f, 0.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f, -0.5f}, .normal = {0.0f, -1.0f, 0.0f}, .texCoord = {0.0f, 1.0f} });
    //right x
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f,  0.5f}, .normal = {1.0f, 0.0f, 0.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f, -0.5f, -0.5f}, .normal = {1.0f, 0.0f, 0.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f, -0.5f}, .normal = {1.0f, 0.0f, 0.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = { 0.5f,  0.5f,  0.5f}, .normal = {1.0f, 0.0f, 0.0f}, .texCoord = {0.0f, 1.0f} });
    //left -x
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f, -0.5f}, .normal = {-1.0f, 0.0f, 0.0f}, .texCoord = {0.0f, 0.0f} });
    mesh.vertices.push_back({ .position = {-0.5f, -0.5f,  0.5f}, .normal = {-1.0f, 0.0f, 0.0f}, .texCoord = {1.0f, 0.0f} });
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f,  0.5f}, .normal = {-1.0f, 0.0f, 0.0f}, .texCoord = {1.0f, 1.0f} });
    mesh.vertices.push_back({ .position = {-0.5f,  0.5f, -0.5f}, .normal = {-1.0f, 0.0f, 0.0f}, .texCoord = {0.0f, 1.0f} });

    // front triangles
    mesh.triangles.push_back({ 0, 1, 2 });
    mesh.triangles.push_back({ 2, 3, 0 });
    //back
    mesh.triangles.push_back({ 4, 5, 6 });
    mesh.triangles.push_back({ 6, 7, 4 });
    //top
    mesh.triangles.push_back({ 8, 9, 10 });
    mesh.triangles.push_back({ 10, 11, 8 });
    //botton
    mesh.triangles.push_back({ 12, 13, 14 });
    mesh.triangles.push_back({ 14, 15, 12 });
    // right
    mesh.triangles.push_back({ 16, 17, 18 });
    mesh.triangles.push_back({ 18, 19, 16 });
    //left
    mesh.triangles.push_back({ 20, 21, 22 });
    mesh.triangles.push_back({ 22, 23, 20 });

    // default material
    mesh.material.kd = glm::vec3(0.8f, 0.8f, 0.8f);

    return mesh;
}

Mesh PrimitiveFactory::createCylinder(int sectors) {
    Mesh mesh;
    const float r = 0.5f;
    const float h = 1.0f;

    float half = h/2.0f;
    for (int j = 0; j < 2; j++) {
        if (j == 1) half = (-1) * half;
        for (int i = 0; i < sectors; i++) {
            float angle = 2.0f * M_PI * i / sectors;
            float x = r * std::cos(angle);
            float z = r * std::sin(angle);

            glm::vec3 pos = { x, half, z };
            glm::vec3 norm = glm::normalize(glm::vec3(x, 0.0f, z));
            glm::vec2 texCoord = { static_cast<float>(i) / sectors, 0.0f };
            mesh.vertices.push_back({ .position = pos, .normal = norm, .texCoord = texCoord });
        }
    }

    // Bottom center cap vertex
    int bci = 2 * sectors;
    mesh.vertices.push_back({ .position = {0.0f, -half, 0.0f}, .normal = {0.0f, -1.0f, 0.0f}, .texCoord = {0.5f, 0.5f} });
    int tci = bci + 1;
    mesh.vertices.push_back({ .position = {0.0f, half, 0.0f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoord = {0.5f, 0.5f} });

    // Side triangles
    for (int i = 0; i < sectors; ++i) {
        int next = (i + 1) % sectors;

        // Bottom triangle
        mesh.triangles.push_back({ static_cast<unsigned int>(i), static_cast<unsigned int>(bci), static_cast<unsigned int>(next) });

        // Top triangle
        mesh.triangles.push_back({ static_cast<unsigned int>(sectors + next), static_cast<unsigned int>(tci), static_cast<unsigned int>(sectors + i) });

        // Side face quads (split into two triangles)
        mesh.triangles.push_back({ static_cast<unsigned int>(i), static_cast<unsigned int>(sectors + i), static_cast<unsigned int>(sectors + next) });
        mesh.triangles.push_back({ static_cast<unsigned int>(i), static_cast<unsigned int>(sectors + next), static_cast<unsigned int>(next) });
    }

    mesh.material.kd = glm::vec3(0.8f, 0.8f, 0.8f);

    return mesh;
}

Mesh PrimitiveFactory::createSphere(int rings, int sectors) {
    Mesh mesh;
    const float r = 0.5f;

    // Generate vertices
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = M_PI * ring / rings;  // 0 to PI

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * M_PI * sector / sectors;  // 0 to 2PI

            // Position on unit sphere
            glm::vec3 position = {
                r * std::sin(phi)* std::cos(theta),
                r * std::cos(phi),
                r * std::sin(phi)* std::sin(theta)
            };

            // Normal is same as position for a sphere centered at origin
            glm::vec3 normal = glm::normalize(position);

            // Texture coordinates
            glm::vec2 texCoord = {
                static_cast<float>(sector) / sectors,
                static_cast<float>(ring) / rings
            };

            mesh.vertices.push_back({ .position = position, .normal = normal, .texCoord = texCoord });
        }
    }

    // Generate triangles
    for (int ring = 0; ring < rings; ++ring) {
        for (int sector = 0; sector < sectors; ++sector) {
            int a = ring * (sectors + 1) + sector;
            int b = a + 1;
            int c = a + (sectors + 1);
            int d = c + 1;

            // First triangle
            mesh.triangles.push_back({ static_cast<unsigned int>(a), static_cast<unsigned int>(c), static_cast<unsigned int>(b) });

            // Second triangle
            mesh.triangles.push_back({ static_cast<unsigned int>(b), static_cast<unsigned int>(c), static_cast<unsigned int>(d) });
        }
    }

    mesh.material.kd = glm::vec3(0.8f, 0.8f, 0.8f);

    return mesh;
}