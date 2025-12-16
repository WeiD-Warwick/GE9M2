#pragma once
#include <string>
#include <map>
#include "../../../../Third_Party/GEMLoader.h"

class RenderContext;
class MeshLibrary;
class ModelData;
struct ModelLoadArgs;

class ModelLoader {
private:
    const std::string                   _prefix = "primitive:";
    MeshLibrary*                        _meshLib;
    RenderContext&                      _renderContext;
    std::map<std::string, ModelData*>   _loadedModelCache;
    GEMLoader::GEMModelLoader           _loader;

public:
    ModelLoader(RenderContext& renderContext);
    ~ModelLoader();

    ModelData* loadModel(const ModelLoadArgs& args);
    MeshLibrary* meshLib();

private:
    ModelData* loadPrimitiveModel(const ModelLoadArgs& args);
    ModelData* loadStaticGEMModel(const ModelLoadArgs& args);
    ModelData* loadAnimatedGEMModel(const ModelLoadArgs& args);
    void listAnimationNames(const GEMLoader::GEMAnimation& gemanimation);
};
