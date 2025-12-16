#pragma once
#include <string>
#include "../Component.h"

class ModelData;
class RenderContext;

class SkySphereRenderComponent : public RenderComponent {

private:
    ModelData* _data;

public:

    SkySphereRenderComponent(ModelData* model);

    ~SkySphereRenderComponent() = default;

    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::Sky; }

    static std::string Name() { return "SkySphereRenderComponent"; }
};
