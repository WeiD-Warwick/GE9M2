#pragma once
#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "../../Third_Party/GEMLoader.h"
#include "Platform/DX12/DX12Resources.h"
#include <string>
#include <map>
#include "Scene/Components/StaticMeshRenderComponent.h"
#include "Scene/Components/AnimatedMeshRenderComponent.h"

class ModelLoader {
    GEMLoader::GEMModelLoader loader;

private:
    RenderContext& _renderContext;

    // Mesh Cache
    std::map<std::string, std::vector<DX12Mesh*>>       _staticMeshCache;
    std::map<std::string, std::vector<DX12Mesh*>>       _animatedMeshCache;
	std::map<std::string, AnimationData*>               _animationCache;
    std::map<std::string, std::vector<std::string>>     _textureFilenameCache;

public:
    ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {}

    ~ModelLoader() {
        for (auto const& pair : _staticMeshCache) {
            for (DX12Mesh* mesh : pair.second) delete mesh;
        }
        for (auto const& pair : _animatedMeshCache) {
            for (DX12Mesh* mesh : pair.second) delete mesh;
        }
        for (auto const& pair : _animationCache) {
            delete pair.second;
        }
    }

    GameObject* generateGameObjectFrom(const std::string& filePath, Scene* scene, bool isAnimated) {
        if (isAnimated) {
            if (_animatedMeshCache.count(filePath)) {
				return loadAnimatedModelFromCache(filePath, scene);
            }
            return loadAnimatedModelFromFile(filePath, scene);
        } 
        else {
            if (_staticMeshCache.count(filePath)) {
                return loadStaticModelFromCache(filePath, scene);
            }
			return loadStaticModelFromFile(filePath, scene);
        }
    }

private:
    GameObject* loadStaticModelFromCache(const std::string& filePath, Scene* scene) {
        GameObject* cachedObject = scene->createObject();
        cachedObject->addComponent<StaticMeshRenderComponent>(
            _staticMeshCache.at(filePath),
            _textureFilenameCache.at(filePath)
        );
        return cachedObject;
	}

    GameObject* loadAnimatedModelFromCache(const std::string& filePath, Scene* scene) {
        GameObject* cachedObject = scene->createObject();
        cachedObject->addComponent<AnimatedMeshRenderComponent>(
            _animatedMeshCache.at(filePath),
            _textureFilenameCache.at(filePath),
            _animationCache.at(filePath)
        );
        return cachedObject;
    }

    GameObject* loadStaticModelFromFile(const std::string& filePath, Scene* scene) {
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        loader.load(filePath, gemmeshes);
        std::vector<std::string> textureFilenames;

        GameObject* object = scene->createObject();
        std::vector<DX12Mesh*> meshes;

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
                textureFilenames.push_back(texName);
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
            meshes.push_back(mesh);
        }

        object->addComponent<StaticMeshRenderComponent>(meshes, textureFilenames);
        _staticMeshCache.insert({ filePath, meshes });
        _textureFilenameCache.insert({ filePath, textureFilenames });
        return object;
	}

    GameObject* loadAnimatedModelFromFile(const std::string& filePath, Scene* scene) {
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        std::vector<std::string> textureFilenames;
        GEMLoader::GEMAnimation gemanimation;
        AnimationData animationData;

        loader.load(filePath, gemmeshes, gemanimation);

        GameObject* object = scene->createObject();
        std::vector<DX12Mesh*> meshes;

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
                textureFilenames.push_back(texName);
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
            meshes.push_back(mesh);
        }

        // load globalInverse
        memcpy(&animationData.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));

        // load bones
        for (auto& gemBone : gemanimation.bones) {
            Bone bone;
            bone.name = gemBone.name;
            memcpy(&bone.offset, &gemBone.offset, 16 * sizeof(float));
            bone.parentIndex = gemBone.parentIndex;
            animationData.skeleton.bones.push_back(bone);
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
            animationData.animations.insert({ name, aseq });
        }

        AnimationData* animationPtr = new AnimationData(std::move(animationData));

		// Cache loaded data
        _animatedMeshCache.insert({ filePath, meshes });
        _textureFilenameCache.insert({ filePath, textureFilenames });
        _animationCache.insert({ filePath, animationPtr });

        object->addComponent<AnimatedMeshRenderComponent>(meshes, textureFilenames, animationPtr);
        return object;
	}

};