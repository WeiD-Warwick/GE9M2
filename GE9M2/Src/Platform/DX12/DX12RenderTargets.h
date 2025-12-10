#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Swapchain.h"
#include <vector>

using Microsoft::WRL::ComPtr;

class DX12RenderTargets {

private:
    ComPtr<ID3D12Device5>                       _device;
    DX12Swapchain*                              _swapChain              = nullptr;

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
        rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
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
