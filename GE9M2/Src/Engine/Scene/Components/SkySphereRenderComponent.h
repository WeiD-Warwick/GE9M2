#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Engine.h"
#include "../Scene.h"

class SkySphereRenderComponent : public Component {

private:
    DX12Mesh*      _mesh;
    std::string    _textureFilename;

    std::string                 _shaderName = "skySphereShader";
    std::string                 _psoName = "skySpherePSO";
    std::string                 _constBufferName = "skySphereBuffer";

public:

    void onRender(RenderContext& renderContext) override {
        if (!_mesh || !owner) return;

        ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
        PSOManager& psos = renderContext.psoManager();
        ShaderManager& shaders = renderContext.shaderManager();
        TextureManager& textureManager = renderContext.textureManager();
        DX12CBVSRVUAVHeap& srvHeap = renderContext.srvHeap();


        Matrix W = Matrix::Translation(owner->scene->mainCamera->position());
        Matrix V = owner->scene->mainCamera->view;
        Matrix P = owner->scene->mainCamera->projection;

        shaders.updateConstantVS(_shaderName, _constBufferName, "W", &W);
        shaders.updateConstantVS(_shaderName, _constBufferName, "V", &V);
        shaders.updateConstantVS(_shaderName, _constBufferName, "P", &P);
        shaders.apply(commandList, _shaderName);
        psos.bind(commandList, _psoName);

        int textureHeapOffet = textureManager.find(_textureFilename);
        shaders.updateTexturePS(commandList, srvHeap, _shaderName, "tex", textureHeapOffet);
        _mesh->draw(commandList);
    }

public:

    SkySphereRenderComponent(
        DX12Mesh* mesh,
        std::string textureFilename
    ) : _mesh(mesh),
        _textureFilename(textureFilename) {
    }

    const std::string& getName() const override {
        static std::string name = "SkySphereRenderComponent";
        return name;
    }

};
