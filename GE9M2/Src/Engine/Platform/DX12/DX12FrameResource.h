#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Fence.h"
#include <vector>

using Microsoft::WRL::ComPtr;

class DX12FrameResource {

private:
    ComPtr<ID3D12CommandAllocator> _graphicsCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList4> _graphicsCommandList;
    DX12Fence _fence;

public:

    void create(ComPtr<ID3D12Device5>& device) {

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_graphicsCommandAllocator));

        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&_graphicsCommandList));

        _graphicsCommandList->Close();

        _fence.create(device);
    }

    void reset() {
        _graphicsCommandAllocator->Reset();
        _graphicsCommandList->Reset(_graphicsCommandAllocator.Get(), NULL);
    }

    void exec(ComPtr<ID3D12CommandQueue>& queue) {
        _graphicsCommandList->Close();
        ID3D12CommandList* lists[] = { _graphicsCommandList.Get() };
        queue->ExecuteCommandLists(1, lists);
    }

    ComPtr<ID3D12GraphicsCommandList4>& commandList() {
        return _graphicsCommandList;
    }

    ComPtr<ID3D12CommandAllocator>& allocator() {
        return _graphicsCommandAllocator;
    }

    DX12Fence& fence() {
        return _fence;
    }

    void flushQueue(ComPtr<ID3D12CommandQueue>& queue) {
        _fence.signal(queue);
        _fence.wait();
    }
};
