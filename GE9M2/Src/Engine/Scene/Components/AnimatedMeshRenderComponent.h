#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Engine.h"
#include "../Scene.h"
#include "../../Graphics/Animation/AnimationData.h"
#include "../../Graphics/Animation/AnimationController.h"

class AnimatedMeshRenderComponent : public Component {

private:
    std::vector<DX12Mesh*>      _meshes;
    std::vector<std::string>    _textureFilenames;

    AnimationData*              _animation               = nullptr;
    AnimationController         _animationController;

    std::string                 _shaderName              = "animatedMeshShader";
    std::string                 _psoName                 = "animatedMeshPSO";
    std::string                 _constBufferName         = "animatedMeshBuffer";

public:

    void onUpdate(float dt) override {
        _animationController.update("run", dt);
        if (_animationController.animationFinished()) {
            _animationController.resetAnimationTime();
        }
	}

    void onRender(RenderContext& renderContext) override {
        if (_meshes.empty() || !owner) return;

        ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
        PSOManager& psos = renderContext.psoManager();
        ShaderManager& shaders = renderContext.shaderManager();
        TextureManager& textureManager = renderContext.textureManager();
		DX12CBVSRVUAVHeap& srvHeap = renderContext.srvHeap();

        Matrix W = owner->transform.worldMatrix();
        Matrix V = owner->scene->mainCamera->view;
        Matrix P = owner->scene->mainCamera->projection;

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

public:

    AnimatedMeshRenderComponent(
        std::vector<DX12Mesh*> meshes,
        std::vector<std::string> textureFilenames,
        AnimationData* animationPtr
    ) : _meshes(meshes), _textureFilenames(textureFilenames), _animation(animationPtr) {
        _animationController.init(_animation);
    }

    ~AnimatedMeshRenderComponent() override {
        _meshes.clear();
        _animation = nullptr;
    }

    const std::string& getName() const override {
        static std::string name = "AnimatedMeshRenderComponent";
        return name;
    }

};
