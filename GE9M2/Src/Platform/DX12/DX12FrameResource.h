#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Fence.h"
#include <vector>

using Microsoft::WRL::ComPtr;

class DX12FrameResource {

private:
    ComPtr<ID3D12CommandAllocator>          _allocator;
    ComPtr<ID3D12GraphicsCommandList4>      _commandList;
    DX12Fence                               _fence;

public:

    void create(ID3D12Device5* device) {

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));

        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&_commandList));

        _fence.create(device);
    }

    void reset() {
		_fence.wait();
        _allocator->Reset();
        _commandList->Reset(_allocator.Get(), NULL);
    }

    void exec(ID3D12CommandQueue* queue) {
        _commandList->Close();
        ID3D12CommandList* lists[] = { _commandList.Get() };
        queue->ExecuteCommandLists(1, lists);
		_fence.signal(queue);
    }

    ID3D12GraphicsCommandList4* commandList() {
        return _commandList.Get();
    }

    ID3D12CommandAllocator* allocator() {
        return _allocator.Get();
    }

    DX12Fence& fence() {
        return _fence;
    }

    void flushQueue(ID3D12CommandQueue* queue) {
        _fence.signal(queue);
        _fence.wait();
    }

public:
    DX12FrameResource() = default;

    DX12FrameResource(const DX12FrameResource&) = delete;
    DX12FrameResource& operator=(const DX12FrameResource&) = delete;

    DX12FrameResource(DX12FrameResource&&) = default;
    DX12FrameResource& operator=(DX12FrameResource&&) = default;
};
