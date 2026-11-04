#include "GasStation.h"
#include "PrimitiveFactory.h"

#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/gtc/matrix_transform.hpp>
DISABLE_WARNINGS_POP()

namespace GasStation {

    void build(std::vector<GPUMesh>& outMeshes, std::vector<glm::mat4>& outTransforms) {
        Mesh cubeMesh = PrimitiveFactory::createCube();

        // Base matrix for the entire shop
        glm::mat4 shopBase = glm::mat4(1.0f);

        // 1. Floor
        glm::mat4 floorModel = glm::translate(shopBase, glm::vec3(0.0f, 0.0f, 0.0f));
        floorModel = glm::scale(floorModel, glm::vec3(10.0f, 0.2f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(floorModel);

        // 2. Back Wall
        glm::mat4 backWall = glm::translate(shopBase, glm::vec3(0.0f, 2.5f, -4.9f));
        backWall = glm::scale(backWall, glm::vec3(10.0f, 5.0f, 0.2f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(backWall);

        // 3. Left Wall
        glm::mat4 leftWall = glm::translate(shopBase, glm::vec3(-4.9f, 2.5f, 0.0f));
        leftWall = glm::scale(leftWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(leftWall);

        // 4. Right Wall
        glm::mat4 rightWall = glm::translate(shopBase, glm::vec3(4.9f, 2.5f, 0.0f));
        rightWall = glm::scale(rightWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(rightWall);

        // 5. Front Wall (Lower half)
        glm::mat4 frontWall = glm::translate(shopBase, glm::vec3(0.0f, 0.5f, 4.9f));
        frontWall = glm::scale(frontWall, glm::vec3(10.0f, 1.0f, 0.2f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frontWall);

        // 6. See-Through Roof
        glm::mat4 roof = glm::translate(shopBase, glm::vec3(0.0f, 5.0f, 0.0f));
        roof = glm::scale(roof, glm::vec3(10.0f, 0.1f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(roof);

        // 7. Checkout Counter
        glm::mat4 counter = glm::translate(shopBase, glm::vec3(-2.0f, 0.75f, -2.0f));
        counter = glm::scale(counter, glm::vec3(3.0f, 1.5f, 1.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(counter);
    }
}