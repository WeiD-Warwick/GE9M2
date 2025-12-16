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

public:
    StaticMeshRenderComponent(ModelData* _data);

    ~StaticMeshRenderComponent() = default;

    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "StaticMeshRenderComponent"; }
};
