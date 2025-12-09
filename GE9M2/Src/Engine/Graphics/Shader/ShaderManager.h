#pragma once
#include <string>
#include <map>
#include "Shader.h"
#include "../../Platform/DX12/DX12CBVSRVUAVHeap.h"

class ShaderManager {
public:
    std::map<std::string, Shader> shaders;

    void load(ID3D12Device5* device, const std::string& name, const std::string& vs, const std::string& ps) {
        if (shaders.find(name) != shaders.end())
            return;

        Shader shader;
        shader.load(device, vs, ps);
        shaders[name] = shader;
    }

    Shader* find(const std::string& name) {
        return &shaders[name];
    }

    void apply(ID3D12GraphicsCommandList4* cmd, const std::string& name) {
        shaders[name].apply(cmd);
    }

    void updateTexturePS(ID3D12GraphicsCommandList4* cmd, DX12CBVSRVUAVHeap& srvHeap, const std::string& shaderName, const std::string& textureName, int heapOffset) {
        UINT bindPoint = shaders[shaderName].textureBindPoints[textureName];
        D3D12_GPU_DESCRIPTOR_HANDLE handle = srvHeap.gpuHandle;

        handle.ptr = handle.ptr + (UINT64)(heapOffset - bindPoint) * (UINT64) srvHeap.incrementSize;
        cmd->SetGraphicsRootDescriptorTable(2, handle);
    }

    void updateConstantVS(std::string shaderName, std::string cbName, std::string vName, void* data) {
        shaders[shaderName].updateVS(cbName, vName, data);
    }

    void updateConstantPS(std::string shaderName, std::string cbName, std::string vName, void* data) {
        shaders[shaderName].updatePS(cbName, vName, data);
    }
};
