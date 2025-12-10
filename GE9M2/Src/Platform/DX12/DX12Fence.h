#pragma once
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// Only have one queue: GraphicesQueue
class DX12Fence {

public:
    ComPtr<ID3D12Fence>     fence;
    HANDLE                  eventHandle     = nullptr;
    UINT64                  value           = 0;

    void create(ID3D12Device5* device) {
        device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void signal(ID3D12CommandQueue* queue) {
        queue->Signal(fence.Get(), ++value);
    }

    void wait() {
        if (fence->GetCompletedValue() < value) {
            fence->SetEventOnCompletion(value, eventHandle);
            WaitForSingleObject(eventHandle, INFINITE);
        }
    }

    ~DX12Fence() {
        if (eventHandle)
            CloseHandle(eventHandle);
    }

public:

    DX12Fence() = default;

    DX12Fence(const DX12Fence&) = delete;
    DX12Fence& operator=(const DX12Fence&) = delete;

    DX12Fence(DX12Fence&&) = default;
    DX12Fence& operator=(DX12Fence&&) = default;

};
