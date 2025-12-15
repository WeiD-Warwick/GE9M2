#pragma once
#include "../Component.h"

using namespace std;

class ModelData;
class Material;

class SkySphereRenderComponent : public RenderComponent {

private:
    ModelData*              _data;
    Material*               _material;
    string                  _psoName = "skySpherePSO";

public:

    SkySphereRenderComponent(ModelData* model, Material* material);

    ~SkySphereRenderComponent();

    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::Sky; }

    static std::string Name() { return "SkySphereRenderComponent"; }
};
