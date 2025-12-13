#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Engine.h"
#include "../Scene.h"

class StaticMeshRenderComponent : public Component {

private:
    std::vector<DX12Mesh*>      _meshes;
    std::vector<std::string>    _textureFilenames;

    std::string                 _shaderName             = "staticMeshShader";
    std::string                 _psoName                = "staticMeshPSO";
    std::string                 _constBufferName        = "staticMeshBuffer";

public:

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
		shaders.apply(commandList, _shaderName);
        psos.bind(commandList, _psoName);

        for (int i = 0; i < _meshes.size(); i++) {
            //int textureHeapOffet = textureManager.find(_textureFilenames[i]);
            //shaders.updateTexturePS(commandList, srvHeap, _shaderName, "tex", textureHeapOffet);
            _meshes[i]->draw(commandList);
        }
    }

public:

    StaticMeshRenderComponent(
        std::vector<DX12Mesh*>& meshes,
        std::vector<std::string>& textureFilenames
    ) : _meshes(meshes),
        _textureFilenames(textureFilenames) {
    }

    const std::string& getName() const override {
        static std::string name = "StaticMeshRenderComponent";
        return name;
    }

};
