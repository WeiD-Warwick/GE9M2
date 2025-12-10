#pragma once
#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "../../Third_Party/GEMLoader.h"
#include "Platform/DX12/DX12Resources.h"
#include <string>
#include <map>
#include "Scene/Components/StaticMeshRenderComponent.h"

class ModelLoader {
    GEMLoader::GEMModelLoader loader;

private:
    RenderContext& _renderContext;

    // Mesh Cache
    std::map<std::string, std::vector<DX12Mesh*>> _meshCache;

public:
    ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {}

    GameObject* load(Scene* scene, const std::string& filename) {
		// Check cache first
        if (_meshCache.count(filename)) {
            GameObject* cachedObject = scene->createObject();
            cachedObject->addComponent<StaticMeshRenderComponent>(_meshCache.at(filename));
            return cachedObject;
        }

		// Load GEM file
        std::vector<GEMLoader::GEMMesh> gemmeshes;
        loader.load(filename, gemmeshes);

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

            mesh->createStatic(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                vertices,
                gemmesh.indices
            );
            meshes.push_back(mesh);
        }

        object->addComponent<StaticMeshRenderComponent>(meshes);
        _meshCache.insert({ filename, meshes });

		return object;
    }

    void clearCache() {
        for (auto& pair : _meshCache) {
            for (DX12Mesh* mesh : pair.second) {
                delete mesh;
            }
        }
        _meshCache.clear();
    }
};