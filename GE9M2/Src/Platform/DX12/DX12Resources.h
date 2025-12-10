#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <vector>
#include "DX12Upload.h"
#include "../../Engine/Foundation/Vertex.h"

using Microsoft::WRL::ComPtr;

class DX12VertexLayoutCache {

public:
    static const D3D12_INPUT_LAYOUT_DESC& getStaticLayout() {
        static const D3D12_INPUT_ELEMENT_DESC inputLayoutStatic[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutStatic, 4 };
        return desc;
    }

    static const D3D12_INPUT_LAYOUT_DESC& getAnimatedLayout() {
        static const D3D12_INPUT_ELEMENT_DESC inputLayoutAnimated[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutAnimated, 6 };
        return desc;
    }
};

class DX12Swapchain {
private:

    ComPtr<IDXGISwapChain3>                 _swapchain;
    std::vector<ComPtr<ID3D12Resource>>     _buffers;
    UINT                                    _bufferCount = 0;

public:

    void create(
        ID3D12CommandQueue* graphicsQueue,
        HWND hwnd,
        UINT width,
        UINT height,
        UINT bufferCount
    ) {

        _bufferCount = bufferCount;
        _buffers.resize(bufferCount);

        ComPtr<IDXGIFactory4> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        // Fill in structure describing SwapChain
        DXGI_SWAP_CHAIN_DESC1 scDesc;
        memset(&scDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.Width = width;
        scDesc.Height = height;
        scDesc.SampleDesc.Count = 1; // MSAA here
        scDesc.SampleDesc.Quality = 0;
        scDesc.BufferCount = bufferCount;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // Create the swapchain (transform swapChain1 to swapChain3)
        ComPtr<IDXGISwapChain1> swapChain1;
        factory->CreateSwapChainForHwnd(
            graphicsQueue,
            hwnd,
            &scDesc,
            nullptr,
            nullptr,
            &swapChain1
        );

        swapChain1.As(&_swapchain);

        // TODO
        for (UINT i = 0; i < bufferCount; i++) {
            _swapchain->GetBuffer(i, IID_PPV_ARGS(&_buffers[i]));
        }
    }

    UINT getCurrentBackBufferIndex() const {
        return _swapchain->GetCurrentBackBufferIndex();
    }

    ID3D12Resource* getBufferResource(UINT index) {
        return _buffers[index].Get();
    }

    void present(UINT sync = 1) {
        _swapchain->Present(sync, 0);
    }

    UINT bufferCount() {
        return _bufferCount;
    }

    IDXGISwapChain3* swapchain() {
        return _swapchain.Get();
    }

    void resizeBuffers(UINT width, UINT height) {

        for (auto& buf : _buffers) {
            buf.Reset();
        }

        _swapchain->ResizeBuffers(
            _bufferCount,
            width,
            height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            0
        );

        _buffers.resize(_bufferCount);
        for (UINT i = 0; i < _bufferCount; i++) {
            _swapchain->GetBuffer(i, IID_PPV_ARGS(&_buffers[i]));
        }
    }

public:
    DX12Swapchain() = default;

    DX12Swapchain(const DX12Swapchain&) = delete;
    DX12Swapchain& operator=(const DX12Swapchain&) = delete;

    DX12Swapchain(DX12Swapchain&&) = default;
    DX12Swapchain& operator=(DX12Swapchain&&) = default;
};

class DX12RenderTargets {

private:
    ComPtr<ID3D12Device5>                       _device;
    DX12Swapchain* _swapChain = nullptr;

    // RTV heap
    ComPtr<ID3D12DescriptorHeap>                _rtvHeap;
    std::vector<ComPtr<ID3D12Resource>>         _backBufferResources;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>    _rtvHandles;

    // DSV heap
    ComPtr<ID3D12DescriptorHeap>                _dsvHeap;
    ComPtr<ID3D12Resource>                      _depthBufferResource;
    D3D12_CPU_DESCRIPTOR_HANDLE                 _dsvHandle;

public:

    ID3D12Resource* backBufferResource(UINT frameIndex) {
        return _backBufferResources[frameIndex].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(UINT frameIndex) {
        return _rtvHandles[frameIndex];
    }

    ID3D12Resource* depthBufferResource() {
        return _depthBufferResource.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle() {
        return _dsvHandle;
    }

    void create(ID3D12Device5* device, DX12Swapchain& swapchain, UINT width, UINT height) {
        _device = device;
        _swapChain = &swapchain;

        createBackBufferResources();
        createDepthBufferResources(width, height);
    }

    void resize(UINT width, UINT height) {
        _backBufferResources.clear();
        _rtvHandles.clear();
        createBackBufferResources();
        createDepthBufferResources(width, height);
    }

private:

    void createBackBufferResources() {
        UINT bufferCount = _swapChain->bufferCount();

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        _device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
        UINT rtvDescSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        _backBufferResources.resize(bufferCount);
        _rtvHandles.resize(bufferCount);
        for (UINT index = 0; index < bufferCount; ++index) {
            _backBufferResources[index] = _swapChain->getBufferResource(index);
            _rtvHandles[index] = rtvHandle;

            _device->CreateRenderTargetView(_backBufferResources[index].Get(), nullptr, _rtvHandles[index]);
            rtvHandle.ptr += rtvDescSize;
        }
    }

    void createDepthBufferResources(UINT width, UINT height) {

        // Create Descriptor Heap
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        _device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));
        _dsvHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
        _depthBufferResource = nullptr;

        // Create Depth Buffer
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthClearValue = {};
        depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.Width = width;
        dsvDesc.Height = height;
        dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        dsvDesc.DepthOrArraySize = 1;
        dsvDesc.MipLevels = 1;
        dsvDesc.SampleDesc.Count = 1;
        dsvDesc.SampleDesc.Quality = 0;
        dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        _device->CreateCommittedResource(
            &heapprops,
            D3D12_HEAP_FLAG_NONE,
            &dsvDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue,
            IID_PPV_ARGS(&_depthBufferResource)
        );

        _device->CreateDepthStencilView(
            _depthBufferResource.Get(),
            &depthStencilDesc,
            _dsvHeap->GetCPUDescriptorHandleForHeapStart()
        );
    }

public:
    DX12RenderTargets() = default;

    DX12RenderTargets(const DX12RenderTargets&) = delete;
    DX12RenderTargets& operator=(const DX12RenderTargets&) = delete;

    DX12RenderTargets(DX12RenderTargets&&) = default;
    DX12RenderTargets& operator=(DX12RenderTargets&&) = default;
};

class DX12CBVSRVUAVHeap {

public:
    ComPtr<ID3D12DescriptorHeap>		heap;
    D3D12_CPU_DESCRIPTOR_HANDLE			cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE			gpuHandle;
    UINT								incrementSize;

    int used;

    void create(ID3D12Device5* device, int num) {

        D3D12_DESCRIPTOR_HEAP_DESC uavcbvHeapDesc = {};
        uavcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        uavcbvHeapDesc.NumDescriptors = num;
        uavcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device->CreateDescriptorHeap(&uavcbvHeapDesc, IID_PPV_ARGS(&heap));
        cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
        gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
        incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        used = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE getNextCPUHandle() {
        if (used > 0) {
            cpuHandle.ptr += incrementSize;
        }
        used++;
        return cpuHandle;
    }

public:
    DX12CBVSRVUAVHeap() = default;

    DX12CBVSRVUAVHeap(const DX12CBVSRVUAVHeap&) = delete;
    DX12CBVSRVUAVHeap& operator=(const DX12CBVSRVUAVHeap&) = delete;

    DX12CBVSRVUAVHeap(DX12CBVSRVUAVHeap&&) = default;
    DX12CBVSRVUAVHeap& operator=(DX12CBVSRVUAVHeap&&) = default;
};

class DX12Mesh {

private:
    ComPtr<ID3D12Resource>          _vertexResource;
    ComPtr<ID3D12Resource>          _indexResource;

    D3D12_VERTEX_BUFFER_VIEW        _vbView;
    D3D12_INDEX_BUFFER_VIEW         _ibView;
    D3D12_INPUT_LAYOUT_DESC         _inputLayoutDesc;

    UINT                            _numIndices = 0;

public:

    // STATIC_VERTEX
    void createStatic(
        ID3D12Device5* device,
        DX12Upload& uploader,
        std::vector<STATIC_VERTEX> vertices,
        std::vector<UINT> indices
    ) {
        init(device, uploader, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
        _inputLayoutDesc = DX12VertexLayoutCache::getStaticLayout();
    }

    // ANIMATED_VERTEX
    void createAnimated(
        ID3D12Device5* device,
        DX12Upload& uploader,
        std::vector<ANIMATED_VERTEX> vertices,
        std::vector<UINT> indices
    ) {
        init(device, uploader, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
        _inputLayoutDesc = DX12VertexLayoutCache::getAnimatedLayout();
    }

    // General Init
    void init(
        ID3D12Device5* device,
        DX12Upload& uploader,
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
