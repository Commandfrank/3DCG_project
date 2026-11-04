#pragma once

#include "framework/mesh.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

class PrimitiveFactory {
public:
    // creates unit cube at (0,0,0)
    static Mesh createCube();
    // creates cylinder at (0,0,0) with height = 1.0 and r = 0.5
    static Mesh createCylinder(int sectors = 16);
    // creates sphere at (0,0,0) wirh r = 0.5
    static Mesh createSphere(int rings = 16, int sectors = 16);
};