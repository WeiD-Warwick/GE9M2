#pragma once
#include "../Component.h"

using namespace std;

class DX12Mesh;
class SkySphereMaterial;

class SkySphereRenderComponent : public Component {

private:
    DX12Mesh*               _mesh;
    SkySphereMaterial*      _material;
    string                  _psoName = "skySpherePSO";

public:

    SkySphereRenderComponent(DX12Mesh* mesh, SkySphereMaterial* material);

    ~SkySphereRenderComponent();

    void onRender(RenderContext& renderContext) override;

};
