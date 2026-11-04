#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <framework/shader.h>
#include "Mesh.h"
#include "PrimitiveFactory.h"

class GPUMesh;

struct BodyParts {
public:
    GPUMesh* gpuMesh = nullptr; // null if this node is just a joint anchor

    // Transformations relative to parent
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f }; // Euler angles in degrees (X, Y, Z)
    glm::vec3 scale{ 1.0f };

    // Visual offset (to shift the primitive relative to the joint pivot)
    glm::vec3 meshOffset{ 0.0f };

    std::vector<std::shared_ptr<BodyParts>> children;

    // Calculates local matrix: Translate -> Rotate -> Scale
    glm::mat4 getLocalTransform() const {
        glm::mat4 mat(1.0f);
        mat = glm::translate(mat, position);
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        return mat;
    }

    void draw(Shader& shader, const glm::mat4& parentTransform, const glm::mat4& viewProjMatrix) {
        glm::mat4 currentGlobal = parentTransform * getLocalTransform();

        if (gpuMesh != nullptr) {
            glm::mat4 modelMatrix = glm::translate(currentGlobal, meshOffset);
            modelMatrix = glm::scale(modelMatrix, scale);

            glm::mat4 mvpMatrix = viewProjMatrix * modelMatrix;
            glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

            // Pass uniforms matching your vertex shader setup
            glUniformMatrix4fv(shader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
            glUniform1i(shader.getUniformLocation("hasTexCoords"), GL_FALSE);
            glUniform1i(shader.getUniformLocation("useMaterial"), GL_TRUE);

            gpuMesh->draw(shader);
        }

        for (auto& child : children) {
            child->draw(shader, currentGlobal, viewProjMatrix);
        }
    }
};

class Human {
private:
    std::unique_ptr<GPUMesh> gpuCube;
    std::unique_ptr<GPUMesh> gpuSphere;
    std::unique_ptr<GPUMesh> gpuCylinder;

    glm::vec3 m_worldPosition{ 0.0f, 1.45f, 0.0f };
    glm::vec3 m_worldRotation{ 0.0f };

    // setup helpers to keep constructor clean
    void initMeshes();
    void buildHierarchy();

public:
    // root node torso
    std::shared_ptr<BodyParts> torsoNode;

    // Direct pointers to key joints for easy pose/animation access
    std::shared_ptr<BodyParts> headNode;
    std::shared_ptr<BodyParts> neck;

    // Arms
    std::shared_ptr<BodyParts> upperLeftArm;
    std::shared_ptr<BodyParts> lowerLeftArm;
    std::shared_ptr<BodyParts> upperRightArm;
    std::shared_ptr<BodyParts> lowerRightArm;

    // Legs
    std::shared_ptr<BodyParts> upperLeftLeg;
    std::shared_ptr<BodyParts> lowerLeftLeg;
    std::shared_ptr<BodyParts> upperRightLeg;
    std::shared_ptr<BodyParts> lowerRightLeg;

    // Constructor & Destructor
    Human();
    ~Human() = default;

    // Rendering method
    void draw(Shader& shader, const glm::mat4& mvpMatrix);

    // Optional pose and animation functions
    void resetPose();
    void setPosition(glm::vec3 coords);
    void turn(float timeInSeconds, float speed = 1.0f, float targetDegree = 90.0f);
    void waveHand(float timeInSeconds);
    void walk(float timeInSeconds, float deltaTime, float animationSpeed=1.0f, float movementSpeed=0.001f);
    void stopMoving();

    //getters setters
    glm::vec3 getPosition() const { return m_worldPosition; }
    glm::vec3 getRotation() const { return m_worldRotation; }
};