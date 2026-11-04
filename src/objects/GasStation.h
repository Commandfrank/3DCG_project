#pragma once

#include "/Users/karolplandowski/CLionProjects/3DCG_project/src/mesh.h"
#include <framework/mesh.h>
#include <vector>
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/glm.hpp>
DISABLE_WARNINGS_POP()

namespace GasStation {
    // Appends the gas station components and their transforms to the provided vectors
    void build(std::vector<GPUMesh>& outMeshes, std::vector<glm::mat4>& outTransforms, std::vector<glm::vec4>& outColors);
}