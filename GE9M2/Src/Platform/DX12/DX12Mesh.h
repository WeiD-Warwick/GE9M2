#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include "DX12UploadContext.h"
#include "DX12VertexLayoutCache.h"
#include "../../Engine/Foundation/Vertex.h"

using Microsoft::WRL::ComPtr;

class DX12Mesh {
private:
    ComPtr<ID3D12Resource>          _vertexResource;
    ComPtr<ID3D12Resource>          _indexResource;

    D3D12_VERTEX_BUFFER_VIEW        _vbView;
    D3D12_INDEX_BUFFER_VIEW         _ibView;
    D3D12_INPUT_LAYOUT_DESC         _inputLayoutDesc;

    UINT                            _numIndices         = 0;

public:

    // STATIC_VERTEX
    void createStatic(
        ID3D12Device5* device,
        DX12UploadContext& uploader,
        std::vector<STATIC_VERTEX> vertices,
        std::vector<UINT> indices
    ) {
        init(device, uploader, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
        _inputLayoutDesc = DX12VertexLayoutCache::getStaticLayout();
    }

    // ANIMATED_VERTEX
    void createAnimated(
        ID3D12Device5* device,
        DX12UploadContext& uploader,
        std::vector<ANIMATED_VERTEX> vertices,
        std::vector<UINT> indices
    ) {
        init(device, uploader, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
        _inputLayoutDesc = DX12VertexLayoutCache::getAnimatedLayout();
    }

    // General Init
    void init(
        ID3D12Device5* device,
        DX12UploadContext& uploader,
        void* vertices,
        int stride,
        int numVertices,
        UINT* indices,
        int numIndices
    ) {

        _numIndices = numIndices;

        UINT vbSize = stride * numVertices;
        UINT ibSize = sizeof(UINT) * numIndices;

        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        // add vertex buffer
        D3D12_RESOURCE_DESC vbDesc = {};
        vbDesc.Width = vbSize;
        vbDesc.Height = 1;
        vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        vbDesc.DepthOrArraySize = 1;
        vbDesc.MipLevels = 1;
        vbDesc.SampleDesc.Count = 1;
        vbDesc.SampleDesc.Quality = 0;
        vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        device->CreateCommittedResource(
            &heapprops,
            D3D12_HEAP_FLAG_NONE, 
            &vbDesc,
            D3D12_RESOURCE_STATE_COMMON,
            NULL,
            IID_PPV_ARGS(&_vertexResource));

        uploader.uploadResource(
            _vertexResource.Get(),
            vertices,
            vbSize,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        // Create view to vertex Buffer
        _vbView.BufferLocation = _vertexResource->GetGPUVirtualAddress();
        _vbView.StrideInBytes = stride;
        _vbView.SizeInBytes = vbSize;

        // Add Index Buffer creation on GPU memory
        D3D12_RESOURCE_DESC ibDesc;
        memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
        ibDesc.Width = ibSize;
        ibDesc.Height = 1;
        ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        ibDesc.DepthOrArraySize = 1;
        ibDesc.MipLevels = 1;
        ibDesc.SampleDesc.Count = 1;
        ibDesc.SampleDesc.Quality = 0;
        ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        device->CreateCommittedResource(
            &heapprops,
            D3D12_HEAP_FLAG_NONE,
            &ibDesc,
            D3D12_RESOURCE_STATE_COMMON,
            NULL,
            IID_PPV_ARGS(&_indexResource));
        
        uploader.uploadResource(
            _indexResource.Get(),
            indices,
            ibSize,
            D3D12_RESOURCE_STATE_INDEX_BUFFER);

        // Create view to Index Buffer
        _ibView.BufferLocation = _indexResource->GetGPUVirtualAddress();
        _ibView.Format = DXGI_FORMAT_R32_UINT;
        _ibView.SizeInBytes = numIndices * sizeof(UINT);
    }

    void draw(ID3D12GraphicsCommandList4* cmd) const {
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd->IASetVertexBuffers(0, 1, &_vbView);
        cmd->IASetIndexBuffer(&_ibView);
        cmd->DrawIndexedInstanced(_numIndices, 1, 0, 0, 0);
    }

public:
    DX12Mesh() = default;

    DX12Mesh(const DX12Mesh&) = delete;
    DX12Mesh& operator=(const DX12Mesh&) = delete;

    DX12Mesh(DX12Mesh&&) = default;
    DX12Mesh& operator=(DX12Mesh&&) = default;
};
