#include "StaticMeshRenderPass.h"
#include "../../Scene/Scene.h"
#include "../RenderContext.h"
#include "../../Scene/Components/CameraComponent.h"
#include "../../Graphics/Model/ModelData.h"

void StaticMeshRenderPass::render(Scene& scene, RenderContext& ctx) {

    auto* camera = scene.mainCamera();
    if (!camera) return;

    auto* cmd = ctx.renderer().commandList();

    MaterialParam param;
    param.VP = camera->view * camera->projection;

    for (const auto& inst : scene.staticMeshInstances()) {

        int instanceCount = (int)inst.worlds.size();
        if (instanceCount == 0) continue;

        ctx.instanceBuffer().upload(inst.worlds.data(), instanceCount);

        for (const auto& sub : inst.model->subMeshes) {

            Material* mat = ctx.materialManager().find(sub.materialKey);

            mat->apply(ctx, param);

            sub.mesh->drawInstanced(
                cmd,
                ctx.instanceBuffer().vertexBufferView(),
                instanceCount
            );
        }
    }
}
