#include "StaticMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"

using namespace std;

StaticMeshRenderComponent::StaticMeshRenderComponent(vector<DX12Mesh*>& meshes, vector<string>& textureFilenames) 
    : _meshes(meshes), _textureFilenames(textureFilenames) {}


void StaticMeshRenderComponent::onRender(RenderContext& renderContext) {
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
    shaders.apply(commandList, _shaderName);
    psos.bind(commandList, _psoName);

    for (int i = 0; i < _meshes.size(); i++) {
        //int textureHeapOffet = textureManager.find(_textureFilenames[i]);
        //shaders.updateTexturePS(commandList, srvHeap, _shaderName, "tex", textureHeapOffet);
        _meshes[i]->draw(commandList);
    }
}