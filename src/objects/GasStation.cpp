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
        outColors.push_back(glm::vec4(1.0f)); // placeholder for brick
        // outColors.push_back(glm::vec4(0.8f, 0.3f, 0.3f, 1.0f)); // red

        // right wall
        glm::mat4 rightWall = glm::translate(shopBase, glm::vec3(4.9f, 2.5f, 0.0f));
        rightWall = glm::scale(rightWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(rightWall);
        outColors.push_back(glm::vec4(1.0f)); // placeholder for brick
        // outColors.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

        // front wall
        glm::mat4 frontWall = glm::translate(shopBase, glm::vec3(0.0f, 2.5f, 4.9f));
        frontWall = glm::scale(frontWall, glm::vec3(10.0f, 5.0f, 0.2f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frontWall);
        outColors.push_back(glm::vec4(1.0f)); // placeholder for brick
        // outColors.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

        // seethrough roof
        glm::mat4 roof = glm::translate(shopBase, glm::vec3(0.0f, 5.0f, 0.0f));
        roof = glm::scale(roof, glm::vec3(10.0f, 0.1f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(roof);
        outColors.push_back(glm::vec4(1.0f));

        // checkout counter
        glm::mat4 counter = glm::translate(shopBase, glm::vec3(-2.9f, 0.75f, -2.0f));
        counter = glm::scale(counter, glm::vec3(3.5f, 1.5f, 1.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(counter);
        outColors.push_back(glm::vec4(0.6f, 0.4f, 0.2f, 1.0f));

        // DOOR FRAME
        glm::vec4 frameColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);

        // frame left post
        glm::mat4 frameLeft = glm::translate(shopBase, glm::vec3(-4.9f, 1.75f, -1.0f));
        frameLeft = glm::scale(frameLeft, glm::vec3(0.25f, 3.5f, 0.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameLeft);
        outColors.push_back(frameColor);

        // frame right post
        glm::mat4 frameRight = glm::translate(shopBase, glm::vec3(-4.9f, 1.75f, 1.0f));
        frameRight = glm::scale(frameRight, glm::vec3(0.25f, 3.5f, 0.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameRight);
        outColors.push_back(frameColor);

        // frame top
        glm::mat4 frameTop = glm::translate(shopBase, glm::vec3(-4.9f, 3.5f, 0.0f));
        frameTop = glm::scale(frameTop, glm::vec3(0.25f, 0.1f, 2.1f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(frameTop);
        outColors.push_back(frameColor);

        // door handle
        glm::mat4 handle = glm::translate(shopBase, glm::vec3(-4.7f, 1.75f, 0.7f));
        handle = glm::scale(handle, glm::vec3(0.15f, 0.3f, 0.08f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(handle);
        outColors.push_back(glm::vec4(0.85f, 0.7f, 0.2f, 1.0f)); //gold

        // SHELVES
        glm::vec4 shelfColor = glm::vec4(0.45f, 0.45f, 0.45f, 1.0f); // Dark Grey
        glm::vec4 productColors[4] = {
            glm::vec4(0.8f, 0.2f, 0.2f, 1.0f), // red box
            glm::vec4(0.2f, 0.8f, 0.2f, 1.0f), // green box
            glm::vec4(0.2f, 0.3f, 0.8f, 1.0f), // blue box
            glm::vec4(0.9f, 0.8f, 0.1f, 1.0f)  // yellow box
        };

        // right wall shelves
        float rightShelfY[3] = { 0.5f, 1.25f, 2.0f }; // heights for 3 shelves
        for (int i = 0; i < 3; ++i) {
            // build the shelf board
            glm::mat4 shelf = glm::translate(shopBase, glm::vec3(4.5f, rightShelfY[i], 0.0f));
            shelf = glm::scale(shelf, glm::vec3(0.6f, 0.05f, 6.0f));
            outMeshes.push_back(GPUMesh(cubeMesh));
            outTransforms.push_back(shelf);
            outColors.push_back(shelfColor);

            // populate products evenly across this shelf
            for (int p = 0; p < 11; ++p) {
                float pZ = -2.5f + (p * 0.5f); // spread from z=-2.5 to z=2.5
                glm::mat4 prod = glm::translate(shopBase, glm::vec3(4.5f, rightShelfY[i] + 0.175f, pZ));
                prod = glm::scale(prod, glm::vec3(0.3f, 0.3f, 0.3f));
                outMeshes.push_back(GPUMesh(cubeMesh));
                outTransforms.push_back(prod);
                // pick a color based on the loop index to make it look mixed
                outColors.push_back(productColors[(i + p) % 4]);
            }
        }

        // double-sided shelf
        glm::mat4 shelfCenter = glm::translate(shopBase, glm::vec3(1.5f, 1.0f, 0.0f));
        shelfCenter = glm::scale(shelfCenter, glm::vec3(0.05f, 2.0f, 5.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(shelfCenter);
        outColors.push_back(shelfColor);

        float midShelfY[3] = { 0.4f, 1.0f, 1.6f };
        float midShelfWidth[3] = { 1.2f, 1.0f, 0.8f }; // shelves get slightly narrower at the top

        for (int i = 0; i < 3; ++i) {
            glm::mat4 shelf = glm::translate(shopBase, glm::vec3(1.5f, midShelfY[i], 0.0f));
            shelf = glm::scale(shelf, glm::vec3(midShelfWidth[i], 0.05f, 5.0f));
            outMeshes.push_back(GPUMesh(cubeMesh));
            outTransforms.push_back(shelf);
            outColors.push_back(shelfColor);

            // left part of shelf
            for (int p = 0; p < 9; ++p) {
                float pZ = -2.0f + (p * 0.5f);
                float pX = 1.5f - (midShelfWidth[i] / 2.0f) + 0.2f; // Push out to the edge
                glm::mat4 prod = glm::translate(shopBase, glm::vec3(pX, midShelfY[i] + 0.175f, pZ));
                prod = glm::scale(prod, glm::vec3(0.25f, 0.3f, 0.25f));
                outMeshes.push_back(GPUMesh(cubeMesh));
                outTransforms.push_back(prod);
                outColors.push_back(productColors[(i + p + 1) % 4]);
            }

            // right part of shelf
            for (int p = 0; p < 9; ++p) {
                float pZ = -2.0f + (p * 0.5f);
                float pX = 1.5f + (midShelfWidth[i] / 2.0f) - 0.2f; // Push out to the edge
                glm::mat4 prod = glm::translate(shopBase, glm::vec3(pX, midShelfY[i] + 0.175f, pZ));
                prod = glm::scale(prod, glm::vec3(0.25f, 0.3f, 0.25f));
                outMeshes.push_back(GPUMesh(cubeMesh));
                outTransforms.push_back(prod);
                outColors.push_back(productColors[(i + p + 2) % 4]);
            }
        }

        // transparent parts have to be initialized after the other stuff

        // transparent left wall
        glm::mat4 leftWall = glm::translate(shopBase, glm::vec3(-4.9f, 2.5f, 0.0f));
        leftWall = glm::scale(leftWall, glm::vec3(0.2f, 5.0f, 10.0f));
        outMeshes.push_back(GPUMesh(cubeMesh));
        outTransforms.push_back(leftWall);
        outColors.push_back(glm::vec4(0.5f, 0.8f, 0.9f, 0.3f));

        // // seethrough roof
        // glm::mat4 roof = glm::translate(shopBase, glm::vec3(0.0f, 5.0f, 0.0f));
        // roof = glm::scale(roof, glm::vec3(10.0f, 0.1f, 10.0f));
        // outMeshes.push_back(GPUMesh(cubeMesh));
        // outTransforms.push_back(roof);
        // outColors.push_back(glm::vec4(0.5f, 0.7f, 1.0f, 0.3f));

    }
}