#pragma once
#include "../Mesh/Mesh.h"
#include "../Material/ModelMaterial.h"
#include "../Animation/AnimationData.h"

struct SubMesh {
public:
    Mesh* mesh = nullptr;
    ModelMaterial* material = nullptr;
};

class ModelData {
public:
    AnimationData* animation = nullptr;
    std::vector<SubMesh>      subMeshes;

    bool isAnimated() { return animation != nullptr; }

    ModelData() = default;

    ModelData(const ModelData&) = delete;
    ModelData& operator=(const ModelData&) = delete;
};

struct ModelLoadArgs {
public:
    std::string filePath;
    std::string shaderName      = "staticMeshShader";
    std::string cbufferName     = "staticMeshBuffer";
    std::string psoName         = "staticMeshPSO";
    std::string textureName     = "";
    Vec2 uvScale                = { 1.0f, 1.0f };
};