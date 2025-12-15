#pragma once
#include <vector>
#include <string>
#include "../Component.h"

class ModelData;
class RenderContext;
class Material;

class StaticMeshRenderComponent : public RenderComponent {

private:
    ModelData*        _data;
    Material*         _material;
    std::string       _psoName                = "staticMeshPSO";

public:
    StaticMeshRenderComponent(ModelData* _data, Material* material);

    ~StaticMeshRenderComponent();

    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "StaticMeshRenderComponent"; }
};
