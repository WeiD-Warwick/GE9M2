#pragma once
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// Only have one queue: GraphicesQueue
class DX12Fence {
public:
    ComPtr<ID3D12Fence> fence;
    HANDLE eventHandle = nullptr;
    UINT64 value = 0;

    void create(ComPtr<ID3D12Device5>& device) {
        device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void signal(ComPtr<ID3D12CommandQueue>& queue) {
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

};
