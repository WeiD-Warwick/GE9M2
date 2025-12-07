#pragma once
#include <d3d12.h>
#include "DX12Fence.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12CommandQueues {
private:
    // Handles rendering commands
    ComPtr<ID3D12CommandQueue> _graphics;

    //// Manages compute tasks
    //ComPtr<ID3D12CommandQueue> _compute;

    //// Dedicated to data transfers
    //// Used for uploading textures, buffers, and other resources without blocking the graphics or ompute queues
    //ComPtr<ID3D12CommandQueue> _copy;

public:
    ComPtr<ID3D12CommandQueue>& graphicsQueue() { return _graphics; }
    //ComPtr<ID3D12CommandQueue>& copyQueue() { return _copy; }
    //ComPtr<ID3D12CommandQueue>& computeQueue() { return _compute; }

    void create(ComPtr<ID3D12Device5>& device) {

        D3D12_COMMAND_QUEUE_DESC desc = {};

        // Graphics
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_graphics));

        //// Copy
        //desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        //device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_copy));

        //// Compute
        //desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        //device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_compute));

    }

};