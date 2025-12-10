#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12Swapchain {
private:

    ComPtr<IDXGISwapChain3>                 _swapchain;
    std::vector<ComPtr<ID3D12Resource>>     _buffers;
    UINT                                    _bufferCount    = 0;

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
