#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Engine.h"
#include "../Scene.h"
#include "../../Graphics/Material/Material.h"

class SkySphereRenderComponent : public Component {

private:
    DX12Mesh*               _mesh;
    SkySphereMaterial*      _material;

    std::string                 _psoName = "skySpherePSO";

public:

    void onRender(RenderContext& renderContext) override {
        if (!_mesh || !owner) return;

        ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
        PSOManager& psos = renderContext.psoManager();

        psos.bind(commandList, _psoName);

        Matrix W = Matrix::Translation(owner->scene->mainCamera->position());
        Matrix V = owner->scene->mainCamera->view;
        Matrix P = owner->scene->mainCamera->projection;
        _material->apply(renderContext, W, V, P);
        _mesh->draw(commandList);
    }

public:

    SkySphereRenderComponent(
        DX12Mesh* mesh,
        SkySphereMaterial* material
    ) : _mesh(mesh),
        _material(material) {
    }

    const std::string& getName() const override {
        static std::string name = "SkySphereRenderComponent";
        return name;
    }

};
