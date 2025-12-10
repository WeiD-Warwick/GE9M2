#pragma once
#include "Skeleton.h"
#include "AnimationClip.h"
#include <unordered_map>
#include <string>

struct AnimationLibrary {
    Skeleton skeleton;

    // multiple clips (Idle, Run, Attack…)
    std::unordered_map<std::string, AnimationClip> clips;

    const AnimationClip* getClip(const std::string& name) const {
        auto it = clips.find(name);
        if (it != clips.end()) return &it->second;
        return nullptr;
    }
};
