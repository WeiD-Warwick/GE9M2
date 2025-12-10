#pragma once
#include <d3d12.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12CBVSRVUAVHeap {

public:
	ComPtr<ID3D12DescriptorHeap>		heap;
	D3D12_CPU_DESCRIPTOR_HANDLE			cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			gpuHandle;
	UINT								incrementSize;

	int used;

	void create(ID3D12Device5* device, int num) {

		D3D12_DESCRIPTOR_HEAP_DESC uavcbvHeapDesc = {};
		uavcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavcbvHeapDesc.NumDescriptors = num;
		uavcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		device->CreateDescriptorHeap(&uavcbvHeapDesc, IID_PPV_ARGS(&heap));
		cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
		incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		used = 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE getNextCPUHandle() {
		if (used > 0) {
			cpuHandle.ptr += incrementSize;
		}
		used++;
		return cpuHandle;
	}

public:
	DX12CBVSRVUAVHeap() = default;

	DX12CBVSRVUAVHeap(const DX12CBVSRVUAVHeap&) = delete;
	DX12CBVSRVUAVHeap& operator=(const DX12CBVSRVUAVHeap&) = delete;

	DX12CBVSRVUAVHeap(DX12CBVSRVUAVHeap&&) = default;
	DX12CBVSRVUAVHeap& operator=(DX12CBVSRVUAVHeap&&) = default;
};