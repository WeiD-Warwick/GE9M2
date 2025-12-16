#include "SkySphereRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Assets/ModelData.h"

SkySphereRenderComponent::SkySphereRenderComponent(ModelData* data) : _data(data) {}

void SkySphereRenderComponent::onRender(RenderContext& renderContext) {

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    param.W = Matrix::Translation(mainCamera()->transform().position);
    param.V = mainCamera()->view.withoutTranslation();
    param.P = mainCamera()->projection;

    for (auto& subMesh : _data->subMeshes) {
        subMesh.material->apply(renderContext, param);
        subMesh.mesh->draw(commandList);
    }
}


