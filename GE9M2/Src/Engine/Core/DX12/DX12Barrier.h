#pragma once
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12Barrier {
public:
    static void add(ComPtr<ID3D12Resource> res, 
                    D3D12_RESOURCE_STATES first,
                    D3D12_RESOURCE_STATES second,
                    ComPtr<ID3D12GraphicsCommandList4> commandList) {

        D3D12_RESOURCE_BARRIER rb = {};
        rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rb.Transition.pResource = res.Get();
        rb.Transition.StateBefore = first;
        rb.Transition.StateAfter = second;
        rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &rb);
    }
};