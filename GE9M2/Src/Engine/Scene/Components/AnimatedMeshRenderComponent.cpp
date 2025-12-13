#include "AnimatedMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"

AnimatedMeshRenderComponent::AnimatedMeshRenderComponent(
    vector<DX12Mesh*>& meshes,
    vector<string>& textureFilenames,
    AnimationData* animationPtr
) : _meshes(meshes), _textureFilenames(textureFilenames), _animation(animationPtr) {
    _animationController.init(_animation);
}

AnimatedMeshRenderComponent::~AnimatedMeshRenderComponent() {
    _meshes.clear();
    _animation = nullptr;
}


void AnimatedMeshRenderComponent::playAnimation(const std::string& name) {
    if (_currentAnimation != name) {
        _currentAnimation = name;
    }
}

void AnimatedMeshRenderComponent::onUpdate(float dt) {
    _animationController.update(_currentAnimation, dt);
    if (_animationController.animationFinished()) {
        _animationController.resetAnimationTime();
    }
}

void AnimatedMeshRenderComponent::onRender(RenderContext& renderContext) {
    if (_meshes.empty() || !_owner) return;

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
    PSOManager& psos = renderContext.psoManager();
    ShaderManager& shaders = renderContext.shaderManager();
    TextureManager& textureManager = renderContext.textureManager();
    DX12CBVSRVUAVHeap& srvHeap = renderContext.srvHeap();

    Matrix W = transform().worldMatrix();
    Matrix V = mainCamera()->view;
    Matrix P = mainCamera()->projection;

    shaders.updateConstantVS(_shaderName, _constBufferName, "W", &W);
    shaders.updateConstantVS(_shaderName, _constBufferName, "V", &V);
    shaders.updateConstantVS(_shaderName, _constBufferName, "P", &P);
    shaders.updateConstantVS(_shaderName, _constBufferName, "bones", _animationController.skinningMatrices);

    shaders.apply(commandList, _shaderName);
    psos.bind(commandList, _psoName);

    for (int i = 0; i < _meshes.size(); i++) {
        int textureHeapOffet = textureManager.find(_textureFilenames[i]);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, "tex", textureHeapOffet);
        _meshes[i]->draw(commandList);
    }
}