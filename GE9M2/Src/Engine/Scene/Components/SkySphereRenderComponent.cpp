#include "SkySphereRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"

using namespace std;

SkySphereRenderComponent::SkySphereRenderComponent(DX12Mesh* mesh, SkySphereMaterial* material)
    : _mesh(mesh), _material(material) {
}

SkySphereRenderComponent::~SkySphereRenderComponent() {
    delete _material;
}

void SkySphereRenderComponent::onRender(RenderContext& renderContext) {
    if (!_mesh || !_owner) return;

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
    PSOManager& psos = renderContext.psoManager();

    psos.bind(commandList, _psoName);

    Matrix W = Matrix::Translation(mainCamera()->transform().position);
    Matrix V = mainCamera()->view;
    Matrix P = mainCamera()->projection;
    _material->apply(renderContext, W, V, P);
    _mesh->draw(commandList);
}


