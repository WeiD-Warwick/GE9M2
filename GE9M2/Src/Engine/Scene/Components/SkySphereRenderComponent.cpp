#include "SkySphereRenderComponent.h"
#include "CameraComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Assets/ModelData.h"
#include "../../Engine.h"

SkySphereRenderComponent::SkySphereRenderComponent(ModelData* data) : _data(data) {}

void SkySphereRenderComponent::onStart() {
    auto& ctx = engine()->renderContext();
    auto& matMgr = ctx.materialManager();

    for (auto& subMesh : _data->subMeshes) {
        _materials.push_back(
            matMgr.createInstance(ctx, subMesh)
        );
    }
}

void SkySphereRenderComponent::onRender(RenderContext& renderContext) {

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    param.W = Matrix::Translation(mainCamera()->transform().position);
    param.V = mainCamera()->view.withoutTranslation();
    param.P = mainCamera()->projection;

    for (int i = 0; i < _data->subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _data->subMeshes[i].mesh->draw(commandList);
    }
}


