#include "SkySphereRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"
#include "../../ModelLoader.h"

SkySphereRenderComponent::SkySphereRenderComponent(ModelData* data, Material* material)
    : _data(data), _material(material) {
}

SkySphereRenderComponent::~SkySphereRenderComponent() {
    delete _material;
}

void SkySphereRenderComponent::onRender(RenderContext& renderContext) {

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
    PSOManager& psos = renderContext.psoManager();

    psos.bind(commandList, _psoName);

    MaterialParam param;
    param.W = Matrix::Translation(mainCamera()->transform().position);
    param.V = mainCamera()->view.withoutTranslation();
    param.P = mainCamera()->projection;

    for (auto& mesh : _data->meshes) {
        _material->apply(renderContext, mesh->textureNames, param);
        mesh->draw(commandList);
    }
}


