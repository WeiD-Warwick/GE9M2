#include "StaticMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Assets/ModelData.h"

StaticMeshRenderComponent::StaticMeshRenderComponent(ModelData* data) : _data(data) {}

void StaticMeshRenderComponent::onRender(RenderContext& renderContext) {
    auto* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    param.W = transform().worldMatrix();
    param.V = mainCamera()->view;
    param.P = mainCamera()->projection;
   
    for (auto& subMesh : _data->subMeshes) {
        subMesh.material->apply(renderContext, param);
        subMesh.mesh->draw(commandList);
    }
}