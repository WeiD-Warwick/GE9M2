#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Barrier.h"
#include "DX12Fence.h" 
#include "DX12FrameResource.h"

using Microsoft::WRL::ComPtr;

class DX12ResourceLoader {
private:
    ComPtr<ID3D12Device5> _device;
    ComPtr<ID3D12CommandQueue> _queue;

    DX12FrameResource _uploadFrame;

public:
    DX12ResourceLoader() = default;


    void create(ComPtr<ID3D12Device5> device, ComPtr<ID3D12CommandQueue> _queue, DX12FrameResource uploadFrame) {
        _device = device;
        _queue = _queue;
        _uploadFrame = uploadFrame;
    }

    void uploadResource(
        ComPtr<ID3D12Resource> dstResource,
        const void* data,
        unsigned int size,
        D3D12_RESOURCE_STATES targetState,
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT* texFootprint = nullptr) {

        ComPtr<ID3D12Resource> uploadBuffer;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = size;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        _device->CreateCommittedResource(&heapProps,
                                         D3D12_HEAP_FLAG_NONE,
                                         &bufferDesc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         NULL,
                                         IID_PPV_ARGS(&uploadBuffer));


        void* mappeddata = nullptr;
        uploadBuffer->Map(0, NULL, &mappeddata);
        memcpy(mappeddata, data, size);
        uploadBuffer->Unmap(0, NULL);

        _uploadFrame.reset();

        ComPtr<ID3D12GraphicsCommandList4> commandList = _uploadFrame.commandList();

        DX12Barrier::add(
            dstResource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST,
            commandList.Get()
        );

        if (texFootprint != nullptr) {
            D3D12_TEXTURE_COPY_LOCATION src = {};
            src.pResource = uploadBuffer.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = *texFootprint;

            D3D12_TEXTURE_COPY_LOCATION dst = {};
            dst.pResource = dstResource.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = 0;

            commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
        }
        else {
            commandList->CopyBufferRegion(dstResource.Get(), 0, uploadBuffer.Get(), 0, size);
        }


        DX12Barrier::add(dstResource.Get(),
                         D3D12_RESOURCE_STATE_COPY_DEST,
                         targetState,
                         commandList.Get());

        _uploadFrame.exec(_queue);
        _uploadFrame.flushQueue(_queue);
    }
};