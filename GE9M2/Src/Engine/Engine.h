#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12CommandQueue.h"
#include "Platform/DX12/DX12Swapchain.h"
#include "Platform/DX12/DX12RenderTargets.h"
#include "Platform/DX12/DX12Renderer.h"
#include "Platform/DX12/DX12UploadContext.h"
#include "Platform/Window/Window.h"

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Engine {
private:
    const int bufferCount = 2;
    int _width = 0;
    int _height = 0;
    HWND _hwnd = nullptr;

public:
    DX12Device device;
    DX12CommandQueues queues;
    DX12Swapchain swapchain;
    DX12RenderTargets renderTargets;
    DX12Renderer renderer;
    DX12UploadContext uploader;

public:
    Engine() = default;

    void init(HWND hwnd, int width, int height) {
        _hwnd = hwnd;
        _width = width;
        _height = height;

        // Create Device
        device.create();
        auto dxDevice = device.device();

        // Create Command Queues
        queues.create(dxDevice);
        auto graphicsQueue = queues.graphicsQueue();

        // Create Swapchain
        swapchain.create(graphicsQueue, hwnd, width, height, 2);

        // Create Render Targets
        renderTargets.create(dxDevice, swapchain, width, height);

        // Create Renderer
        renderer.create(dxDevice, graphicsQueue, swapchain, renderTargets, width, height);

        // Create Upload Context
        uploader.create(dxDevice, graphicsQueue);
    }

    void beginFrame() {
        renderer.beginFrame();
    }

    void endFrame() {
        renderer.endFrame();
    }

    void flush() {
        renderer.flushGraphicsQueue();
    }

    ComPtr<ID3D12GraphicsCommandList4> cmd() {
        return renderer.commandList();
    }

    DX12UploadContext& upload() {
        return uploader;
    }
};