#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12Swapchain {
private:
    ComPtr<IDXGISwapChain3> _swapchain;
    std::vector<ComPtr<ID3D12Resource>> _buffers;
    UINT _bufferCount = 0;

public:

    void create(ComPtr<ID3D12CommandQueue> graphicsQueue, HWND hwnd, UINT width, UINT height, UINT bufferCount) {

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
        factory->CreateSwapChainForHwnd(graphicsQueue.Get(), hwnd, &scDesc, NULL, NULL, &swapChain1);
        swapChain1.As(&_swapchain);

        for (UINT i = 0; i < bufferCount; i++) {
            _swapchain->GetBuffer(i, IID_PPV_ARGS(&_buffers[i]));
        }

    }

    UINT getCurrentBackBufferIndex() const {
        return _swapchain->GetCurrentBackBufferIndex();
    }

    ComPtr<ID3D12Resource>& getBufferResource(UINT index) {
        return _buffers[index];
    }

    void present(UINT sync = 1) {
        _swapchain->Present(sync, 0);
    }

    UINT bufferCount() {
        return _bufferCount;
    }

    ComPtr<IDXGISwapChain3> swapchain() {
        return _swapchain;
    }
};
