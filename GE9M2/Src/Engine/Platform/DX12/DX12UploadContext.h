#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Barrier.h"
#include "DX12Fence.h"

using Microsoft::WRL::ComPtr;

class DX12UploadContext {
private:
    ComPtr<ID3D12Device5> _device;
    ComPtr<ID3D12CommandQueue> _queue;

    ComPtr<ID3D12CommandAllocator> _allocator;
    ComPtr<ID3D12GraphicsCommandList4> _commandList;

    DX12Fence _fence;

public:
    void create(ComPtr<ID3D12Device5> device, ComPtr<ID3D12CommandQueue> queue) {
        _device = device;
        _queue = queue;

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));

        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&_commandList));

        _commandList->Close();

        _fence.create(device);
    }

    // Upload a generic buffer
    void uploadBuffer(ComPtr<ID3D12Resource> dst, 
                      const void* data, 
                      UINT size, 
                      D3D12_RESOURCE_STATES targetState) {
        ComPtr<ID3D12Resource> uploadBuffer;
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = size;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        _device->CreateCommittedResource(&heapProps,
                                         D3D12_HEAP_FLAG_NONE,
                                         &desc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         nullptr,
                                         IID_PPV_ARGS(&uploadBuffer));

        void* mappeddata = nullptr;
        uploadBuffer->Map(0, NULL, &mappeddata);
        memcpy(mappeddata, data, size);
        uploadBuffer->Unmap(0, NULL);

        _allocator->Reset();
        _commandList->Reset(_allocator.Get(), nullptr);

        DX12Barrier::add(dst, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, _commandList);

        _commandList->CopyBufferRegion(dst.Get(), 0, uploadBuffer.Get(), 0, size);

        DX12Barrier::add(dst, D3D12_RESOURCE_STATE_COPY_DEST, targetState, _commandList);

        _commandList->Close();

        ID3D12CommandList* lists[] = { _commandList.Get() };
        _queue->ExecuteCommandLists(1, lists);

        _fence.signal(_queue);
        _fence.wait();
    }
};
