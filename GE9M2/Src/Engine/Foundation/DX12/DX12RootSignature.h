#pragma once

#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>
#include <string>

using Microsoft::WRL::ComPtr;

class DX12RootSignature {

private:
    ComPtr<ID3D12RootSignature>         _rootSignature;

public:

    int srvRootIndex = INT_MAX;

    // RootParam 0: VS CBV(b0)
    // RootParam 1: PS CBV(b0)
    // RootParam 2: SRV Table (t0–t7)
    ID3D12RootSignature* rootSignature() const { return _rootSignature.Get(); }

    void create(ID3D12Device5* device) {
        std::vector<D3D12_ROOT_PARAMETER> params;

        // VS CBV(b0)
        D3D12_ROOT_PARAMETER meshVS_CBV{};
        meshVS_CBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        meshVS_CBV.Descriptor.ShaderRegister = 0;
        meshVS_CBV.Descriptor.RegisterSpace = 0;
        meshVS_CBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        params.push_back(meshVS_CBV);

        //D3D12_ROOT_PARAMETER lightVS_CBV{};
        //lightVS_CBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        //lightVS_CBV.Descriptor.ShaderRegister = 1;
        //lightVS_CBV.Descriptor.RegisterSpace = 0;
        //lightVS_CBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        //params.push_back(lightVS_CBV);

        // PS CBV(b0)
        D3D12_ROOT_PARAMETER meshPS_CBV{};
        meshPS_CBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        meshPS_CBV.Descriptor.ShaderRegister = 0;
        meshPS_CBV.Descriptor.RegisterSpace = 0;
        meshPS_CBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params.push_back(meshPS_CBV);

        // PS CBV(b1)
        //D3D12_ROOT_PARAMETER lightPS_CBV{};
        //lightPS_CBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        //lightPS_CBV.Descriptor.ShaderRegister = 1;
        //lightPS_CBV.Descriptor.RegisterSpace = 0;
        //lightPS_CBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        //params.push_back(lightPS_CBV);

        // SRV Table (t0–t7)
        srvRootIndex = params.size();
        D3D12_DESCRIPTOR_RANGE srvRange = {};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 8;        // number of SRVs (t0–t7)
        srvRange.BaseShaderRegister = 0;    // starting at t0
        srvRange.RegisterSpace = 0;
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER rootParameterTex;
        rootParameterTex.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameterTex.DescriptorTable.NumDescriptorRanges = 1;
        rootParameterTex.DescriptorTable.pDescriptorRanges = &srvRange;
        rootParameterTex.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params.push_back(rootParameterTex);

        D3D12_STATIC_SAMPLER_DESC staticSampler = {};
        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0;
        staticSampler.MaxAnisotropy = 1;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0;
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.NumParameters = (UINT)params.size();
        desc.pParameters = params.data();
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        desc.NumStaticSamplers = 1;
        desc.pStaticSamplers = &staticSampler;

        ComPtr<ID3DBlob> serialized;
        ComPtr<ID3DBlob> errors;

        D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &serialized,
            &errors
        );

        device->CreateRootSignature(
            0,
            serialized->GetBufferPointer(),
            serialized->GetBufferSize(),
            IID_PPV_ARGS(&_rootSignature)
        );
    }

public:
    DX12RootSignature() = default;

    DX12RootSignature(const DX12RootSignature&) = delete;
    DX12RootSignature& operator=(const DX12RootSignature&) = delete;

    DX12RootSignature(DX12RootSignature&&) = default;
    DX12RootSignature& operator=(DX12RootSignature&&) = default;
};