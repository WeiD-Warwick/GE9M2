#pragma once
#include "../../../Third_Party/GEMLoader.h"

class GEMImporter {
public:
    static bool loadStaticMesh(const std::string& path, MeshData& mesh);
    static bool loadAnimatedMesh(const std::string& path, MeshData& mesh, Skeleton& sk, std::vector<AnimationClip>& anims);
};
