#pragma once
#include "../Foundation/Maths.h"
#include <vector>

// A single frame of animation (all bones)
struct BoneTransform {
    Vec3 position;
    Quaternion rotation;
    Vec3 scaling;
};

struct AnimationFrame {
    std::vector<BoneTransform> bones;  // one entry per bone
};