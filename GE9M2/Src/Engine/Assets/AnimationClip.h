#pragma once
#include "AnimationFrame.h"
#include <string>
#include <vector>

// AnimationClip = single named animation (Idle, Run, Attack, etc.)
struct AnimationClip {
    std::string name;
    float ticksPerSecond = 24.0f;
    std::vector<AnimationFrame> frames;
};
