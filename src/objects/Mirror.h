#pragma once

#include <memory>
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNINGS_POP()

#include <framework/shader.h>
#include "mesh.h"
#include "PrimitiveFactory.h"

class Mirror {
public:
    Mirror();

    // World Transformation Getters/Setters
    void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);
    void setRotation(const glm::vec3& rotationDegrees);

    glm::vec3 getPosition() const { return m_worldPosition; }
    glm::vec3 getScale() const { return m_worldScale; }
    glm::vec3 getRotation() const { return m_worldRotation; }

    // Renders the mirror sphere using the active shader and view-projection matrix
    void draw(Shader& mirrorShader, const glm::mat4& viewProjMatrix, const glm::vec3& cameraPos);


private:
    void initMesh();

    std::unique_ptr<GPUMesh> gpuSphere;

    // Transformation parameters
    glm::vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_worldScale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 m_worldRotation{ 0.0f, 0.0f, 0.0f }; // Euler angles in degrees
};