#include "Mirror.h"

Mirror::Mirror() {
    initMesh();
}

void Mirror::initMesh() {
    Mesh sphereMesh = PrimitiveFactory::createSphere(32, 32); // High resolution
    gpuSphere = std::make_unique<GPUMesh>(sphereMesh);
}

void Mirror::setPosition(const glm::vec3& position) {
    m_worldPosition = position;
}

void Mirror::setScale(const glm::vec3& scale) {
    m_worldScale = scale;
}

void Mirror::setRotation(const glm::vec3& rotationDegrees) {
    m_worldRotation = rotationDegrees;
}

void Mirror::draw(Shader& mirrorShader, const glm::mat4& viewProjMatrix, const glm::vec3& cameraPos) {
    if (!gpuSphere) return;

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, m_worldPosition);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(m_worldRotation.z), glm::vec3(0, 0, 1));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(m_worldRotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(m_worldRotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, m_worldScale);

    glm::mat4 mvpMatrix = viewProjMatrix * modelMatrix;
    glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

    // binding active reflection shader
    mirrorShader.bind();
    glUniformMatrix4fv(mirrorShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(mirrorShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    GLint normalLoc = mirrorShader.getUniformLocation("normalModelMatrix");
    if (normalLoc != -1) {
        glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
    }

    //camera location
    GLint camLoc = mirrorShader.getUniformLocation("cameraPos");
    if (camLoc != -1) {
        glUniform3fv(camLoc, 1, glm::value_ptr(cameraPos));
    }

    GLint texLoc = mirrorShader.getUniformLocation("hasTexCoords");
    if (texLoc != -1) glUniform1i(texLoc, GL_FALSE);

    GLint matLoc = mirrorShader.getUniformLocation("useMaterial");
    if (matLoc != -1) glUniform1i(matLoc, GL_TRUE);

    gpuSphere->draw(mirrorShader);
}