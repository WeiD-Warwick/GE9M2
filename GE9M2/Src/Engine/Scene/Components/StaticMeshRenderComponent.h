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
    std::vector<Material*> _materials;

public:
    StaticMeshRenderComponent(ModelData* _data);

    ~StaticMeshRenderComponent() = default;

    void onStart() override;

    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "StaticMeshRenderComponent"; }
};
