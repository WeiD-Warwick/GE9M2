#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Platform/DX12/DX12Mesh.h"
#include "../../Engine.h"
#include "../Scene.h"

class StaticMeshRenderComponent : public Component {

private:
    DX12Mesh* _mesh = nullptr;

public:

    const std::string& getName() const override {
        static std::string name = "StaticMeshRenderComponent";
        return name;
    }

    void setMesh(DX12Mesh* mesh) {
        _mesh = mesh;
    }

    void onRender(RenderContext& renderContext) override {
        if (!_mesh || !owner) return;
        
		ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();
		PSOManager& psos = renderContext.psoManager();
        ShaderManager& shaders = renderContext.shaderManager();

        Matrix M = owner->transform.worldMatrix();
		Matrix V = owner->scene->mainCamera->view;
		Matrix P = owner->scene->mainCamera->projection;

        shaders.updateConstantVS("staticMesh", "staticMeshBuffer", "M", &M);
        shaders.updateConstantVS("staticMesh", "staticMeshBuffer", "V", &V);
        shaders.updateConstantVS("staticMesh", "staticMeshBuffer", "P", &P);
		shaders.apply(commandList, "staticMesh");
        psos.bind(commandList, "staticMeshPSO");
        _mesh->draw(commandList);
    }
};
