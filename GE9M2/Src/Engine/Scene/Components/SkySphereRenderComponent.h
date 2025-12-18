#pragma once
#include <string>
#include <vector>
#include "../Component.h"

class ModelData;
class RenderContext;
class Material;

class SkySphereRenderComponent : public RenderComponent {

private:
    ModelData*        _data;
    std::vector<Material*> _materials;

public:

    SkySphereRenderComponent(ModelData* model);

    ~SkySphereRenderComponent() = default;

    void onStart() override;

    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::Sky; }

    static std::string Name() { return "SkySphereRenderComponent"; }
};
