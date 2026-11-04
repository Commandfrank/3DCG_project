#include "GasStation.h"
#include "PrimitiveFactory.h"

#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/gtc/matrix_transform.hpp>
DISABLE_WARNINGS_POP()

namespace GasStation {

    void build(std::vector<GPUMesh>& outMeshes, std::vector<glm::mat4>& outTransforms,std::vector<glm::vec4>& outColors) {
        Mesh cubeMesh = PrimitiveFactory::createCube();
        // Base matrix for the entire shop
        glm::mat4 shopBase = glm::mat4(1.0f);

        // floor
        glm::mat4 floorModel = glm::translate(shopBase, glm::vec3(0.0f, 0.0f, 0.0f));
        floorModel = glm::scale(floorModel, glm::vec3(10.0f, 0.2f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(floorModel);
        outColors.push_back(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

        // back wall
        glm::mat4 backWall = glm::translate(shopBase, glm::vec3(0.0f, 2.5f, -4.9f));
        backWall = glm::scale(backWall, glm::vec3(10.0f, 5.0f, 0.2f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(backWall);
        // outColors.push_back(glm::vec4(0.8f, 0.3f, 0.3f, 1.0f)); // red
        outColors.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

        // right wall
        glm::mat4 rightWall = glm::translate(shopBase, glm::vec3(4.9f, 2.5f, 0.0f));
        rightWall = glm::scale(rightWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(rightWall);
        outColors.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

        // checkout counter
        glm::mat4 counter = glm::translate(shopBase, glm::vec3(-2.0f, 0.75f, -2.0f));
        counter = glm::scale(counter, glm::vec3(3.0f, 1.5f, 1.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(counter);
        outColors.push_back(glm::vec4(0.6f, 0.4f, 0.2f, 1.0f));


        // --- DOOR FRAME (Dark Aluminum Frame) ---
        glm::vec4 frameColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);

        // Frame Left Post
        glm::mat4 frameLeft = glm::translate(shopBase, glm::vec3(-4.9f, 1.75f, -1.0f));
        frameLeft = glm::scale(frameLeft, glm::vec3(0.25f, 3.5f, 0.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameLeft);
        outColors.push_back(frameColor);

        // Frame Right Post
        glm::mat4 frameRight = glm::translate(shopBase, glm::vec3(-4.9f, 1.75f, 1.0f));
        frameRight = glm::scale(frameRight, glm::vec3(0.25f, 3.5f, 0.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameRight);
        outColors.push_back(frameColor);

        // Frame Top Lintel
        glm::mat4 frameTop = glm::translate(shopBase, glm::vec3(-4.9f, 3.5f, 0.0f));
        frameTop = glm::scale(frameTop, glm::vec3(0.25f, 0.1f, 2.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameTop);
        outColors.push_back(frameColor);

        // Door Handle (Metallic Yellow/Gold)
        glm::mat4 handle = glm::translate(shopBase, glm::vec3(-4.7f, 1.75f, 0.7f));
        handle = glm::scale(handle, glm::vec3(0.15f, 0.3f, 0.08f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(handle);
        outColors.push_back(glm::vec4(0.85f, 0.7f, 0.2f, 1.0f));



        // transparent parts have to be initialized after the other stuff

        // transparent left wall
        glm::mat4 leftWall = glm::translate(shopBase, glm::vec3(-4.9f, 2.5f, 0.0f));
        leftWall = glm::scale(leftWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(leftWall);
        // outColors.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // white
        outColors.push_back(glm::vec4(0.5f, 0.8f, 0.9f, 0.3f));

        // seethrough roof
        glm::mat4 roof = glm::translate(shopBase, glm::vec3(0.0f, 5.0f, 0.0f));
        roof = glm::scale(roof, glm::vec3(10.0f, 0.1f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(roof);
        // outColors.push_back(glm::vec4(0.5f, 0.7f, 1.0f, 0.3f));

        // front wall (Lower half)
        glm::mat4 frontWall = glm::translate(shopBase, glm::vec3(0.0f, 0.5f, 4.9f));
        frontWall = glm::scale(frontWall, glm::vec3(10.0f, 1.0f, 0.2f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frontWall);
        outColors.push_back(glm::vec4(0.5f, 0.8f, 0.9f, 0.3f));

    }
}