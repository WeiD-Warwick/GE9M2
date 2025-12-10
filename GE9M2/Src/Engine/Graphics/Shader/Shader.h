#pragma once
#include <string>
#include <vector>
#include <map>
#include <d3d12.h>
#include "ShaderCompiler.h"
#include "../ConstantBuffer.h"


class Shader {
public:
    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;

    ShaderCompiler compiler;

    std::vector<ConstantBuffer> vsCBs;
    std::vector<ConstantBuffer> psCBs;
    std::map<std::string, int> textureBindPoints;

    void load(ID3D12Device5* device, const std::string& vsPath, const std::string& psPath) {
        vs = compiler.compileOrLoad(vsPath, "VS", "vs_5_0");
        ps = compiler.compileOrLoad(psPath, "PS", "ps_5_0");

        reflect(device, vs.Get(), vsCBs);
        reflect(device, ps.Get(), psCBs);
    }

    void apply(ID3D12GraphicsCommandList4* cmd) {
        for (int i = 0; i < vsCBs.size(); i++) {
            cmd->SetGraphicsRootConstantBufferView(0, vsCBs[i].getGPUAddress());
            vsCBs[i].next();
        }
        for (int i = 0; i < psCBs.size(); i++) {
            cmd->SetGraphicsRootConstantBufferView(1, psCBs[i].getGPUAddress());
            psCBs[i].next();
        }
    }

    void updateVS(const std::string& cbName, const std::string& var, void* data) {
        update(cbName, var, data, vsCBs);
    }

    void updatePS(const std::string& cbName, const std::string& var, void* data) {
        update(cbName, var, data, psCBs);
    }


private:
    void reflect(ID3D12Device5* device, ID3DBlob* blob, std::vector<ConstantBuffer>& buffers) {

        ComPtr<ID3D12ShaderReflection> reflection;
        D3DReflect(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            IID_PPV_ARGS(&reflection)
        );

        D3D12_SHADER_DESC desc;
        reflection->GetDesc(&desc);

        for (UINT i = 0; i < desc.ConstantBuffers; i++) {
            auto* cbReflection = reflection->GetConstantBufferByIndex(i);

            D3D12_SHADER_BUFFER_DESC cbDesc;
            cbReflection->GetDesc(&cbDesc);

            ConstantBuffer cb;
            cb.name = cbDesc.Name;
            UINT totalSize = 0;

            for (UINT n = 0; n < cbDesc.Variables; ++n) {
                auto* var = cbReflection->GetVariableByIndex(n);
                D3D12_SHADER_VARIABLE_DESC vDesc;
                var->GetDesc(&vDesc);

                ConstantBufferVariable cbVariable;
                cbVariable.offset = vDesc.StartOffset;
                cbVariable.size = vDesc.Size;
                cb.cbData.insert({ vDesc.Name, cbVariable });
                totalSize += vDesc.Size;
            }

            cb.create(device, totalSize);
            buffers.push_back(cb);
        }

        for (UINT i = 0; i < desc.BoundResources; i++) {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            reflection->GetResourceBindingDesc(i, &bindDesc);
            if (bindDesc.Type == D3D_SIT_TEXTURE) {
                textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });
            }
        }
    }

    void update(const std::string& cbName, const std::string& var, void* data, std::vector<ConstantBuffer>& buffers) {
        for (auto& cb : buffers) {
            if (cb.name == cbName) {
                cb.update(var, data);
                return;
            }
        }
    }
};
