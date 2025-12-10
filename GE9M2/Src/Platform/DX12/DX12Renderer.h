#pragma once
#include <d3d12.h>
#include "DX12Swapchain.h"
#include "DX12Fence.h"
#include "DX12Barrier.h"
#include "DX12RenderTargets.h"
#include "DX12FrameResource.h"
#include "DX12RootSignature.h"
#include "DX12CBVSRVUAVHeap.h"
#include "../../Graphics/Shader/ShaderManager.h"
#include "../../Graphics/Pipeline/PSOManager.h"
#include "../../Graphics/Texture/TextureManager.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>
#include "DX12VertexLayoutCache.h"

using Microsoft::WRL::ComPtr;

class DX12Renderer {

private:
    ComPtr<ID3D12Device5>           _device;
    ComPtr<ID3D12CommandQueue>      _graphicsQueue;

    DX12Swapchain*                  _swapchain         = nullptr;
    DX12RenderTargets*              _targets           = nullptr;
    DX12CBVSRVUAVHeap*              _srvHeap           = nullptr;
    DX12RootSignature*              _rootSignature     = nullptr;
    ShaderManager*		            _shaderManager     = nullptr;
	PSOManager*                     _psoManager        = nullptr;
    TextureManager*                 _textureManager    = nullptr;

    D3D12_VIEWPORT                  _viewport          = {};
    D3D12_RECT                      _scissor           = {};

    std::vector<DX12FrameResource>  _frameResources;

public:

    void create(
        ID3D12Device5* device,
        ID3D12CommandQueue* graphicsQueue,
        DX12Swapchain& swapchain,
        DX12RenderTargets& targets,
        DX12RootSignature& rootSignature,
        DX12CBVSRVUAVHeap& srvHeap,
        ShaderManager& shaderManager,
        PSOManager& psoManager,
        TextureManager& textureManager,
        int width,
        int height
    ) {
        _device = device;
        _graphicsQueue = graphicsQueue;

        _swapchain = &swapchain;
        _targets = &targets;
        _rootSignature = &rootSignature;
        _srvHeap = &srvHeap;
		_shaderManager = &shaderManager;
        _psoManager = &psoManager;
		_textureManager = &textureManager;

        _viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
        _scissor = { 0, 0, width, height };

        _frameResources.resize(_swapchain->bufferCount());
        for (auto& frameResource : _frameResources) {
            frameResource.create(_device.Get());
        }
    }

    void beginFrame() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Fence& fence = frameResource.fence();
        ID3D12GraphicsCommandList4* commandList = frameResource.commandList();
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = _targets->rtvHandle(frameIndex);
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = _targets->dsvHandle();

        frameResource.reset();

        DX12Barrier::add(
            _targets->backBufferResource(frameIndex),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            commandList
        );

        float color[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
        commandList->ClearRenderTargetView(_targets->rtvHandle(frameIndex), color, 0, nullptr);
        commandList->ClearDepthStencilView(_targets->dsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    
        ID3D12DescriptorHeap* heaps[] = { _srvHeap->heap.Get() };
        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_scissor);
        commandList->SetDescriptorHeaps(1, heaps);
        commandList->SetGraphicsRootSignature(_rootSignature->rootSignature());
    }

    void endFrame() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Barrier::add(
            _targets->backBufferResource(frameIndex),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            frameResource.commandList()
        );

        frameResource.exec(_graphicsQueue.Get());
        _swapchain->present();
    }

    ID3D12GraphicsCommandList4* commandList() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        return _frameResources[frameIndex].commandList();
    }

    void flushGraphicsQueue() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        _frameResources[frameIndex].flushQueue(_graphicsQueue.Get());
    }

public:

    DX12Renderer() = default;
    
    DX12Renderer(const DX12Renderer&) = delete;
    DX12Renderer& operator=(const DX12Renderer&) = delete;
    
    DX12Renderer(DX12Renderer&&) = default;
    DX12Renderer& operator=(DX12Renderer&&) = default;
};
