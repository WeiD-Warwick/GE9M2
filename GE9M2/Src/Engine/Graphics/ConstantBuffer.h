#pragma once
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <map>
#include <string>
#include <wrl/client.h>

#pragma comment(lib, "d3d12.lib")

using Microsoft::WRL::ComPtr;

struct ConstantBufferVariable {
public:
    UINT offset;
    UINT size;
};

class ConstantBuffer {

public:
    ComPtr<ID3D12Resource>       constantBuffer;
    unsigned char*               mappedData         = nullptr;
    std::string                  name;
    UINT                         cbSizeInBytes      = 0;
    UINT                         offsetIndex        = 0;
    UINT                         numInstances       = 0;

    std::map<std::string, ConstantBufferVariable>   cbData;

    void create(ID3D12Device5* device, UINT sizeInBytes, UINT maxDrawCalls = 1024) {

        cbSizeInBytes = (sizeInBytes + 255) & ~255;
        unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
        numInstances = maxDrawCalls;
        offsetIndex = 0;

        D3D12_HEAP_PROPERTIES heapprops;
        memset(&heapprops, 0, sizeof(D3D12_HEAP_PROPERTIES));
        heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC cbDesc;
        memset(&cbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
        cbDesc.Width = cbSizeInBytesAligned;
        cbDesc.Height = 1;
        cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        cbDesc.DepthOrArraySize = 1;
        cbDesc.MipLevels = 1;
        cbDesc.SampleDesc.Count = 1;
        cbDesc.SampleDesc.Quality = 0;
        cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        device->CreateCommittedResource(&heapprops,
            D3D12_HEAP_FLAG_NONE,
            &cbDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            NULL,
            IID_PPV_ARGS(&constantBuffer)
        );

        D3D12_RANGE readRange = { 0, 0 };
        constantBuffer->Map(0, &readRange, (void**)&mappedData);
    }

    void update(const std::string& name, void* data) {
        ConstantBufferVariable cbVariable = cbData[name];
        unsigned int offset = offsetIndex * cbSizeInBytes;
        memcpy(&mappedData[offset + cbVariable.offset], data, cbVariable.size);
    }

    D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const {
        return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
    }

    void next() {
        offsetIndex++;
        if (offsetIndex >= numInstances) {
            offsetIndex = 0;
        }
    }
};