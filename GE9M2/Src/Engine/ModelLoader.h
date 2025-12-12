#pragma once
#include <string>
#include <map>
#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Animation/AnimationData.h"
#include "Platform/DX12/DX12Resources.h"
#include "../../Third_Party/GEMLoader.h"
#include "Scene/Component.h"
#include "Scene/Components/StaticMeshRenderComponent.h"
#include "Scene/Components/AnimatedMeshRenderComponent.h"

class LoadedModelData {
public:

    // Only for animated models
    AnimationData* animation = nullptr;
    std::vector<DX12Mesh*>      meshes;
    std::vector<std::string>    textureFilenames;

    bool isAnimated() { return animation != nullptr; }

    LoadedModelData() = default;

    ~LoadedModelData() {
        for (DX12Mesh* mesh : meshes) {
            delete mesh;
        }

        if (animation != nullptr) {
            delete animation;
            animation = nullptr;
        }
    }

    LoadedModelData(const LoadedModelData&) = delete;
    LoadedModelData& operator=(const LoadedModelData&) = delete;
};

class ModelLoader {

private:
    GEMLoader::GEMModelLoader loader;
    RenderContext& _renderContext;
    std::map<std::string, LoadedModelData*> _loadedModelCache;

public:
    ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {}

    GameObject* generateGameObjectFrom(const std::string& filePath, Scene* scene) {
        GameObject* cachedObject = scene->createObject();
        LoadedModelData* data = loadModelFromFile(filePath);
        if (data->isAnimated()) {
            cachedObject->addComponent<AnimatedMeshRenderComponent>(
                data->meshes, data->textureFilenames, data->animation);
        }
        else {
            cachedObject->addComponent<StaticMeshRenderComponent>(data->meshes, data->textureFilenames);
        }
        return cachedObject;
    }

    LoadedModelData* loadModelFromFile(const std::string& filePath) {
        if (_loadedModelCache.count(filePath)) {
            return _loadedModelCache.at(filePath);
        }

        bool isAnimated = loader.isAnimatedModel(filePath);
        if (isAnimated) {
            return loadAnimatedModelFromFile(filePath);
        }
        else {
            return loadStaticModelFromFile(filePath);
        }
    }

private:
    LoadedModelData* loadStaticModelFromFile(const std::string& filePath) {
        LoadedModelData* data = new LoadedModelData();
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        loader.load(filePath, gemmeshes);

        for (auto& gemmesh : gemmeshes) {
            DX12Mesh* mesh = new DX12Mesh();
            std::vector<STATIC_VERTEX> vertices;

            for (auto& gemVertex : gemmesh.verticesStatic) {
                STATIC_VERTEX vertex;
                memcpy(&vertex, &gemVertex, sizeof(STATIC_VERTEX));
                vertices.push_back(vertex);
            }

            std::string texName = gemmesh.material.find("albedo").getValue();
            if (!texName.empty()) {
                std::string fullPath = "Src/Assets/Models/Textures/" + texName;
                data->textureFilenames.push_back(texName);
                _renderContext.textureManager().loadTexture(
                    _renderContext.device().dxDevice(),
                    _renderContext.uploader(),
                    _renderContext.srvHeap(),
                    texName,
                    fullPath
                );
            }

            mesh->createStatic(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                vertices,
                gemmesh.indices
            );
            data->meshes.push_back(mesh);
        }
        _loadedModelCache.insert({ filePath, data });
        return data;
    }

    LoadedModelData* loadAnimatedModelFromFile(const std::string& filePath) {
        LoadedModelData* data = new LoadedModelData();
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        GEMLoader::GEMAnimation gemanimation;
        data->animation = new AnimationData();

        loader.load(filePath, gemmeshes, gemanimation);

        // Load meshes
        for (auto& gemmesh : gemmeshes) {
            DX12Mesh* mesh = new DX12Mesh();
            std::vector<ANIMATED_VERTEX> animatedVertices;

            for (auto& gemAnimatedVertex : gemmesh.verticesAnimated) {
                ANIMATED_VERTEX vanimatedVertex;
                memcpy(&vanimatedVertex, &gemAnimatedVertex, sizeof(ANIMATED_VERTEX));
                animatedVertices.push_back(vanimatedVertex);
            }

            std::string texName = gemmesh.material.find("albedo").getValue();
            if (!texName.empty()) {
                std::string fullPath = "Src/Assets/" + texName;
                data->textureFilenames.push_back(texName);
                _renderContext.textureManager().loadTexture(
                    _renderContext.device().dxDevice(),
                    _renderContext.uploader(),
                    _renderContext.srvHeap(),
                    texName,
                    fullPath
                );
            }

            mesh->createAnimated(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                animatedVertices,
                gemmesh.indices
            );
            data->meshes.push_back(mesh);
        }

        // load globalInverse
        memcpy(
            &data->animation->skeleton.globalInverse,
            &gemanimation.globalInverse,
            sizeof(Matrix));

        // load bones
        for (auto& gemBone : gemanimation.bones) {
            Bone bone;
            bone.name = gemBone.name;
            memcpy(&bone.offset, &gemBone.offset, sizeof(Matrix));
            bone.parentIndex = gemBone.parentIndex;
            data->animation->skeleton.bones.push_back(bone);
        }

        // load animation data
        for (auto& gemAnimation : gemanimation.animations) {
            std::string name = gemAnimation.name;
            AnimationSequence aseq;
            aseq.ticksPerSecond = gemAnimation.ticksPerSecond;
            for (auto& gemFrame : gemAnimation.frames) {
                AnimationFrame frame;
                for (int index = 0; index < gemFrame.positions.size(); index++) {
                    Vec3 p;
                    memcpy(&p, &gemFrame.positions[index], sizeof(Vec3));
                    Quaternion q;
                    memcpy(&q, &gemFrame.rotations[index], sizeof(Quaternion));
                    Vec3 s;
                    memcpy(&s, &gemFrame.scales[index], sizeof(Vec3));

                    frame.boneLocalTransforms.push_back(Transform(p, q, s));
                }
                aseq.frames.push_back(frame);
            }
            data->animation->animations.insert({ name, aseq });
        }

        _loadedModelCache.insert({ filePath, data });
        return data;
    }

public:
    ~ModelLoader() {
        for (auto const& pair : _loadedModelCache) {
            delete pair.second;
        }
        _loadedModelCache.clear();
    }
};