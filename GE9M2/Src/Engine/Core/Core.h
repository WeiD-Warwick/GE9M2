#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <vector>
#include "DX12/DX12Barrier.h"
#include "DX12/DX12Fence.h"
#include "DX12/DX12Device.h"
#include "DX12/DX12CommandQueue.h"
#include "DX12/DX12Renderer.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")

class Core {
public:

    const int bufferCount = 2;

    DX12Device _device;
    DX12CommandQueues _commandQueues;
    DX12Swapchain _swapchain;
    DX12RenderTargets _renderTargets;
    DX12Renderer _renderer;

    // rootSignature
    ID3D12RootSignature* rootSignature = nullptr;

    unsigned int srvTableIndex;
    int _width;
    int _height;
    HWND windowHandle;


    void init(HWND hwnd, int width, int height) {
        windowHandle = hwnd;
        _width = width;
        _height = height;

        _device.create();

        ComPtr<ID3D12Device5> device = _device.device();
        
        _commandQueues.create(device);

        ComPtr<ID3D12CommandQueue> graphicsQueue = _commandQueues.graphicsQueue();

        _swapchain.create(graphicsQueue, hwnd, width, height, bufferCount);

        _renderTargets.create(device, _swapchain, width, height);

        _renderer.create(device, graphicsQueue, _swapchain, _renderTargets, width, height);
    }

    void beginFrame() {
        _renderer.beginFrame();
    }

    void finishFrame() {
        _renderer.finishFrame();
    }

    void flushGraphicsQueue() {
        _renderer.flushGraphicsQueue();
    }
};