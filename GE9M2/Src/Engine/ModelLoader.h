#pragma once
#include <string>
#include <map>
#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Animation/AnimationData.h"
#include "Platform/DX12/DX12Resources.h"
#include "../../Third_Party/GEMLoader.h"
#include "Foundation/Transform.h"
#include "Graphics/Mesh/MeshLib.h"

class ModelData {
public:

    // Only for animated models
    AnimationData* animation = nullptr;
    std::vector<DX12Mesh*>      meshes;

    bool isAnimated() { return animation != nullptr; }

    ModelData() = default;

    ~ModelData() {
        for (DX12Mesh* mesh : meshes) {
            delete mesh;
        }

        if (animation != nullptr) {
            delete animation;
            animation = nullptr;
        }
    }

    ModelData(const ModelData&) = delete;
    ModelData& operator=(const ModelData&) = delete;
};

class ModelLoader {

private:
    GEMLoader::GEMModelLoader         loader;
    MeshLibrary*		              _meshLib;
    RenderContext&                    _renderContext;
    std::map<std::string, ModelData*> _loadedModelCache;

public:
    MeshLibrary* meshLib() { return _meshLib; }
    
    ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {
        _meshLib = new MeshLibrary(renderContext);
    }

    ModelData* loadModelFromFile(const std::string& filePath) {

        if (_loadedModelCache.count(filePath)) {
            return _loadedModelCache.at(filePath);
        }

        if (filePath == "primitive:plane") {
            DX12Mesh* plane = &_meshLib->plane;
            plane->textureNames.push_back("Src/Assets/Textures/ground_diffuse.png");
            ModelData* data = new ModelData();
            data->meshes = { plane };
            _loadedModelCache.insert({ filePath, data });
            return data;
        }
        else if (filePath == "primitive:sphere") {
            DX12Mesh* skySphere = &_meshLib->skySphere;
            skySphere->textureNames.push_back("Src/Assets/Textures/skySphere.png");
            ModelData* data = new ModelData();
            data->meshes = { skySphere };
            _loadedModelCache.insert({ filePath, data });
            return data;
        }
        else if (filePath == "primitive:cube") {
            DX12Mesh* cube = &_meshLib->cube;
            cube->textureNames.push_back("Src/Assets/Textures/ground_diffuse.png");
            ModelData* data = new ModelData();
            data->meshes = { cube };
            _loadedModelCache.insert({ filePath, data });
            return data;
        }
        else {
            bool isAnimated = loader.isAnimatedModel(filePath);
            if (isAnimated) {
                return loadAnimatedModelFromFile(filePath);
            }
            else {
                return loadStaticModelFromFile(filePath);
            }
        }
    }

private:
    ModelData* loadStaticModelFromFile(const std::string& filePath) {
        ModelData* data = new ModelData();
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
                // Add texture to mesh
                mesh->textureNames.push_back(texName);
                // Koad texture to textureManager
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

    ModelData* loadAnimatedModelFromFile(const std::string& filePath) {
        ModelData* data = new ModelData();
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        GEMLoader::GEMAnimation gemanimation;
        data->animation = new AnimationData();

        loader.load(filePath, gemmeshes, gemanimation);

        listAnimationNames(gemanimation);

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
                // Add texture to mesh
                mesh->textureNames.push_back(texName);
                // Koad texture to textureManager
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

    void listAnimationNames(const GEMLoader::GEMAnimation& gemanimation) {
        for (int i = 0; i < gemanimation.animations.size(); i++)
        {
            std::cout << gemanimation.animations[i].name << std::endl;
        }
    }

public:
    ~ModelLoader() {
        for (auto const& pair : _loadedModelCache) {
            delete pair.second;
        }
        _loadedModelCache.clear();
    }
};