#include "StaticMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Model/ModelData.h"
#include "../../Engine.h"

StaticMeshRenderComponent::StaticMeshRenderComponent(ModelData* data) : _data(data) {}

void StaticMeshRenderComponent::onStart() {
    auto& ctx = engine()->renderContext();
    auto& matMgr = ctx.materialManager();

    for (auto& subMesh : _data->subMeshes) {
        _materials.push_back(
            matMgr.createInstance(ctx, subMesh)
        );
    }
}

void StaticMeshRenderComponent::onRender(RenderContext& renderContext) {
    auto* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    param.W = transform().worldMatrix();
    param.VP = mainCamera()->view * mainCamera()->projection;
   
    for (int i = 0; i < _data->subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _data->subMeshes[i].mesh->draw(commandList);
    }
}