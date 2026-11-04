#include "Cashier.h"
#include <glm/gtc/type_ptr.hpp>


Cashier::Cashier() {
    initMeshes();
    buildHierarchy();
}

void Cashier::initMeshes() {
    Mesh cubeMesh = PrimitiveFactory::createCube();
    Mesh sphereMesh = PrimitiveFactory::createSphere(16, 16);
    Mesh cylinderMesh = PrimitiveFactory::createCylinder(16);

    // converting to gpu mesh
    gpuCube = std::make_unique<GPUMesh>(cubeMesh);
    gpuCylinder = std::make_unique<GPUMesh>(cylinderMesh);
    gpuSphere = std::make_unique<GPUMesh>(sphereMesh);
}

void Cashier::buildHierarchy() {
    //TORSO root 
    torsoNode = std::make_shared<BodyParts>();
    torsoNode-> gpuMesh = gpuCube.get();    
    torsoNode-> position = glm::vec3(0.0f, 1.2f, 0.0f); // Base position above ground
    torsoNode-> scale = glm::vec3(0.6f, 0.8f, 0.3f);    // Torso dimensions (W, H, D)
    torsoNode-> meshOffset = glm::vec3(0.0f, 0.0f, 0.0f); // Centered on torso root

    // Dimensions referenced for child placement
    float torsoWidth = torsoNode->scale.x;
    float torsoHeight = torsoNode->scale.y;

    // HEAD
    headNode = std::make_shared<BodyParts>();
    headNode->gpuMesh = gpuSphere.get();    
    headNode->scale = glm::vec3(0.4f, 0.4f, 0.4f);
    // Positioned at top of torso
    headNode->position = glm::vec3(0.0f, torsoHeight * 0.5f + 0.2f, 0.0f);
    torsoNode->children.push_back(headNode);

    
    // LEFT ARM
    // upper left (pivot at shoulder)
    upperLeftArm = std::make_shared<BodyParts>();
    upperLeftArm->gpuMesh = gpuCylinder.get();    
    upperLeftArm->scale = glm::vec3(0.12f, 0.4f, 0.12f);
    upperLeftArm->position = glm::vec3(torsoWidth * 0.5f + 0.08f, torsoHeight * 0.4f, 0.0f);
    upperLeftArm->meshOffset = glm::vec3(0.0f, -0.2f, 0.0f); // Extend downward from pivot
    torsoNode->children.push_back(upperLeftArm);

    // lower left (pivot elbow)
    lowerLeftArm = std::make_shared<BodyParts>();
    lowerLeftArm->gpuMesh = gpuCylinder.get();
    lowerLeftArm->scale = glm::vec3(0.10f, 0.4f, 0.10f);
    lowerLeftArm->position = glm::vec3(0.0f, -0.4f, 0.0f); // Attached to bottom of upper arm
    lowerLeftArm->meshOffset = glm::vec3(0.0f, -0.2f, 0.0f);
    upperLeftArm->children.push_back(lowerLeftArm);


    //  RIGHT ARM
    // upper right (pivot shoulder)
    upperRightArm = std::make_shared<BodyParts>();
    upperRightArm->gpuMesh = gpuCylinder.get();
    upperRightArm->scale = glm::vec3(0.12f, 0.4f, 0.12f);
    upperRightArm->position = glm::vec3(-(torsoWidth * 0.5f + 0.08f), torsoHeight * 0.4f, 0.0f);
    upperRightArm->meshOffset = glm::vec3(0.0f, -0.2f, 0.0f);
    torsoNode->children.push_back(upperRightArm);

    // lower right (pivot elbow)
    lowerRightArm = std::make_shared<BodyParts>();
    lowerRightArm->gpuMesh = gpuCylinder.get();
    lowerRightArm->scale = glm::vec3(0.10f, 0.4f, 0.10f);
    lowerRightArm->position = glm::vec3(0.0f, -0.4f, 0.0f);
    lowerRightArm->meshOffset = glm::vec3(0.0f, -0.2f, 0.0f);
    upperRightArm->children.push_back(lowerRightArm);


    // LEFT LEG
    // upper left (pivot hip)
    upperLeftLeg = std::make_shared<BodyParts>();
    upperLeftLeg->gpuMesh = gpuCylinder.get();
    upperLeftLeg->scale = glm::vec3(0.15f, 0.5f, 0.15f);
    upperLeftLeg->position = glm::vec3(torsoWidth * 0.25f, -torsoHeight * 0.5f, 0.0f);
    upperLeftLeg->meshOffset = glm::vec3(0.0f, -0.25f, 0.0f);
    torsoNode->children.push_back(upperLeftLeg);

    // lower left (pivot at knee)
    lowerLeftLeg = std::make_shared<BodyParts>();
    lowerLeftLeg->gpuMesh = gpuCylinder.get();
    lowerLeftLeg->scale = glm::vec3(0.13f, 0.5f, 0.13f);
    lowerLeftLeg->position = glm::vec3(0.0f, -0.5f, 0.0f);
    lowerLeftLeg->meshOffset = glm::vec3(0.0f, -0.25f, 0.0f);
    upperLeftLeg->children.push_back(lowerLeftLeg);


    // RIGHT LEG
    // up right (pivot hip)
    upperRightLeg = std::make_shared<BodyParts>();
    upperRightLeg->gpuMesh = gpuCylinder.get();
    upperRightLeg->scale = glm::vec3(0.15f, 0.5f, 0.15f);
    upperRightLeg->position = glm::vec3(-torsoWidth * 0.25f, -torsoHeight * 0.5f, 0.0f);
    upperRightLeg->meshOffset = glm::vec3(0.0f, -0.25f, 0.0f);
    torsoNode->children.push_back(upperRightLeg);

    // low right (pivot knee)
    lowerRightLeg = std::make_shared<BodyParts>();
    lowerRightLeg->gpuMesh = gpuCylinder.get();   
    lowerRightLeg->scale = glm::vec3(0.13f, 0.5f, 0.13f);
    lowerRightLeg->position = glm::vec3(0.0f, -0.5f, 0.0f);
    lowerRightLeg->meshOffset = glm::vec3(0.0f, -0.25f, 0.0f);
    upperRightLeg->children.push_back(lowerRightLeg);
}

void Cashier::draw(Shader& shader, const glm::mat4& viewProjMatrix) {
    if (torsoNode) {
        glm::mat4 identity(1.0f);
        torsoNode->draw(shader, identity, viewProjMatrix);
    }
}

void Cashier::resetPose() {
    // Reset all joint rotations to 0 degrees
    upperLeftArm->rotation = glm::vec3(0.0f);
    lowerLeftArm->rotation = glm::vec3(0.0f);
    upperRightArm->rotation = glm::vec3(0.0f);
    lowerRightArm->rotation = glm::vec3(0.0f);

    upperLeftLeg->rotation = glm::vec3(0.0f);
    lowerLeftLeg->rotation = glm::vec3(0.0f);
    upperRightLeg->rotation = glm::vec3(0.0f);
    lowerRightLeg->rotation = glm::vec3(0.0f);

    headNode->rotation = glm::vec3(0.0f);
}

void Cashier::waveHand(float timeInSeconds) {
    // Raise upper left arm outwards
    upperLeftArm->rotation.z = 120.0f;

    // Wave lower arm back and forth along Z-axis
    lowerLeftArm->rotation.z = 30.0f + std::sin(timeInSeconds * 6.0f) * 25.0f;
}

void Cashier::walk(float timeInSeconds, float speed) {
    float t = timeInSeconds * speed * 4.0f;

    // Hip rotations (X-axis pitch, opposite limbs out of phase)
    float legAngle = std::sin(t) * 30.0f;
    upperLeftLeg->rotation.x = legAngle;
    upperRightLeg->rotation.x = -legAngle;

    // Knees bend only backward when moving backward
    lowerLeftLeg->rotation.x = (legAngle > 0.0f) ? std::sin(t) * 35.0f : 0.0f;
    lowerRightLeg->rotation.x = (-legAngle > 0.0f) ? std::sin(t + M_PI) * 35.0f : 0.0f;

    // Arm swing opposite to leg movement
    upperLeftArm->rotation.x = -legAngle * 0.8f;
    upperRightArm->rotation.x = legAngle * 0.8f;

    // Slight elbow bend
    lowerLeftArm->rotation.x = -15.0f;
    lowerRightArm->rotation.x = -15.0f;
}