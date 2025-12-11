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

        Matrix M = owner->transform.worldMatrix();
		Matrix V = owner->scene->mainCamera->view;
		Matrix P = owner->scene->mainCamera->projection;

        shaders.updateConstantVS(_shaderName, _constBufferName, "M", &M);
        shaders.updateConstantVS(_shaderName, _constBufferName, "V", &V);
        shaders.updateConstantVS(_shaderName, _constBufferName, "P", &P);
		shaders.apply(commandList, _shaderName);
        psos.bind(commandList, _psoName);

        for (auto& mesh : _meshes) {
			mesh->draw(commandList);
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
