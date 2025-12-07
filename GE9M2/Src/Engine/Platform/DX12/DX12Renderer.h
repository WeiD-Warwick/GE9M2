#pragma once
#include <d3d12.h>
#include "DX12Swapchain.h"
#include "DX12Fence.h"
#include "DX12Barrier.h"
#include "DX12RenderTargets.h"
#include "DX12FrameResource.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12Renderer {
public:
    static const UINT FrameCount = 2;

private:
    ComPtr<ID3D12Device5> _device;
    ComPtr<ID3D12CommandQueue> _graphicsQueue;

    DX12Swapchain _swapchain;
    DX12RenderTargets _targets;

    std::vector<DX12FrameResource> _frameResources;

    D3D12_VIEWPORT _viewport = {};
    D3D12_RECT _scissor = {};

    ComPtr<ID3D12RootSignature> _rootSignature;

    unsigned int srvTableIndex;

public:

    void create(
        ComPtr<ID3D12Device5> device,
        ComPtr<ID3D12CommandQueue> graphicsQueue,
        DX12Swapchain swapchain,
        DX12RenderTargets targets,
        int width, int height) {
        _device = device;
        _graphicsQueue = graphicsQueue;
        _swapchain = swapchain;
        _targets = targets;
        _viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
        _scissor = { 0, 0, width, height };

        _frameResources.resize(swapchain.bufferCount());

        for (auto& frameResource : _frameResources) {
            frameResource.create(device);
        }

        createRootSignature();
    }

    void beginFrame() {
        UINT frameIndex = _swapchain.getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Fence& fence = frameResource.fence();
        ComPtr<ID3D12CommandAllocator>& allocator = frameResource.allocator();
        ComPtr<ID3D12GraphicsCommandList4>& commandList = frameResource.commandList();
        
        fence.wait();
        frameResource.reset();

        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_scissor);

        DX12Barrier::add(
            _targets.backBufferResource(frameIndex).Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            commandList.Get()
        );

        
        commandList->OMSetRenderTargets(1, &_targets.rtvHandle(frameIndex), FALSE, &_targets.dsvHandle());

        float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
        commandList->ClearRenderTargetView(_targets.rtvHandle(frameIndex), color, 0, nullptr);
        commandList->ClearDepthStencilView(_targets.dsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    void endFrame() {
        UINT frameIndex = _swapchain.getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Fence& fence = frameResource.fence();
        ComPtr<ID3D12CommandAllocator>& allocator = frameResource.allocator();
        ComPtr<ID3D12GraphicsCommandList4>& commandList = frameResource.commandList();

        DX12Barrier::add(
            _targets.backBufferResource(frameIndex).Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            commandList.Get()
        );

        // Submit commands
        frameResource.exec(_graphicsQueue);

        // Signal
        fence.signal(_graphicsQueue);

        // Present
        _swapchain.present();
    }

    void beginRenderPass() {
        UINT frameIndex = _swapchain.getCurrentBackBufferIndex();
        ComPtr<ID3D12GraphicsCommandList4>& commandList = _frameResources[frameIndex].commandList();

        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_scissor);
        commandList->SetGraphicsRootSignature(_rootSignature.Get());
    }

    ComPtr<ID3D12GraphicsCommandList4>& commandList() {
        UINT frameIndex = _swapchain.getCurrentBackBufferIndex();
        return _frameResources[frameIndex].commandList();
    }

    void flushGraphicsQueue() {
        UINT frameIndex = _swapchain.getCurrentBackBufferIndex();
        _frameResources[frameIndex].flushQueue(_graphicsQueue);
    }

private:
    void createRootSignature() {
        std::vector<D3D12_ROOT_PARAMETER> parameters;
        D3D12_ROOT_PARAMETER rootParameterCBVS;
        rootParameterCBVS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameterCBVS.Descriptor.ShaderRegister = 0; // Register(b0)
        rootParameterCBVS.Descriptor.RegisterSpace = 0;
        rootParameterCBVS.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        parameters.push_back(rootParameterCBVS);
        D3D12_ROOT_PARAMETER rootParameterCBPS;
        rootParameterCBPS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameterCBPS.Descriptor.ShaderRegister = 0; // Register(b0)
        rootParameterCBPS.Descriptor.RegisterSpace = 0;
        rootParameterCBPS.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        parameters.push_back(rootParameterCBPS);

        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = parameters.size();
        desc.pParameters = &parameters[0];
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        ComPtr<ID3DBlob> serialized;
        ComPtr<ID3DBlob> error;
        D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
        _device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
        srvTableIndex = 1;
    }

};