#pragma once
#include "../Component.h"

using namespace std;

class ModelData;
class Material;

class SkySphereRenderComponent : public Component {

private:
    ModelData*              _data;
    Material*               _material;
    string                  _psoName = "skySpherePSO";

public:

    SkySphereRenderComponent(ModelData* model, Material* material);

    ~SkySphereRenderComponent();

    void onRender(RenderContext& renderContext) override;

};
