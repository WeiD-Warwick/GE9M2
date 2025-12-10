#pragma once
#include "../Foundation/Maths.h"
#include <string>
#include <vector>

struct Bone {
    std::string name;
    Matrix offset;        // inverse bind pose (bone offset)
    int parent = -1;      // parent index (-1 = root)
};

struct Skeleton {
    std::vector<Bone> bones;
    Matrix globalInverse; // scene root inverse
};
