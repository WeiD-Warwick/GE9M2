#include "AnimatedMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Assets/ModelData.h"

AnimatedMeshRenderComponent::AnimatedMeshRenderComponent(ModelData* data) : _data(data) {
    _animationController.init(_data->animation);
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
    param.V = mainCamera()->view;
    param.P = mainCamera()->projection;
    param.bones = _animationController.skinningMatrices;

    for (auto& subMesh : _data->subMeshes) {
        subMesh.material->apply(renderContext, param);
        subMesh.mesh->draw(commandList);
    }
}