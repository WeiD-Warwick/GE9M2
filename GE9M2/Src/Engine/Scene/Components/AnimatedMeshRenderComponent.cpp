#include "AnimatedMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Model/ModelData.h"
#include "../../Engine.h"

AnimatedMeshRenderComponent::AnimatedMeshRenderComponent(ModelData* data) : _data(data) {
    _animationController.init(_data->animation);
}

void AnimatedMeshRenderComponent::onStart() {
    auto& ctx = engine()->renderContext();
    auto& matMgr = ctx.materialManager();

    for (auto& subMesh : _data->subMeshes) {
        _materials.push_back(
            matMgr.createInstance(ctx, subMesh)
        );
    }
}

void AnimatedMeshRenderComponent::playAnimation(const std::string& name) {
    if (_currentAnimation != name) {
        _currentAnimation = name;
    }
}

void AnimatedMeshRenderComponent::onUpdate(float dt) {
    //_animationController.update(dt);
    //_animationController.play(_currentAnimation);
    //if (_animationController.animationFinished()) {
    //    _animationController.resetAnimationTime();
    //}
}

void AnimatedMeshRenderComponent::onRender(RenderContext& renderContext) {
    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    param.W = transform().worldMatrix();
    param.VP = mainCamera()->view * mainCamera()->projection;
    param.bones = _animationController.skinningMatrices;

    for (int i = 0; i < _data->subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _data->subMeshes[i].mesh->draw(commandList);
    }
}