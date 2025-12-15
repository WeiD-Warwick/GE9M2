#include "AnimatedMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../ModelLoader.h"

AnimatedMeshRenderComponent::AnimatedMeshRenderComponent(ModelData* data, Material* material)
    : _data(data), _material(material) {
    _animationController.init(_data->animation);
}

AnimatedMeshRenderComponent::~AnimatedMeshRenderComponent() { delete _material; }

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
    PSOManager& psos = renderContext.psoManager();
    psos.bind(commandList, _psoName);

    MaterialParam param;
    param.W = transform().worldMatrix();
    param.V = mainCamera()->view;
    param.P = mainCamera()->projection;
    param.bones = _animationController.skinningMatrices;

    for (auto& mesh : _data->meshes) {
        _material->apply(renderContext, mesh->textureNames, param);
        mesh->draw(commandList);
    }
}