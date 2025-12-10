#pragma once
#include "../../Platform/DX12/DX12Device.h"
#include "../../Platform/DX12/DX12CommandQueue.h"
#include "../../Platform/DX12/DX12Swapchain.h"
#include "../../Platform/DX12/DX12RenderTargets.h"
#include "../../Platform/DX12/DX12CBVSRVUAVHeap.h"
#include "../../Platform/DX12/DX12RootSignature.h"
#include "../../Platform/DX12/DX12Renderer.h"
#include "../../Platform/DX12/DX12UploadContext.h"
#include "PSOManager.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureManager.h"

class RenderContext {
private:
    const int bufferCount = 2;
    const int srvHeapSize = 16384;

    DX12Device              _device;
    DX12CommandQueues       _queues;
    DX12Swapchain           _swapchain;
    DX12RenderTargets       _targets;
    DX12CBVSRVUAVHeap       _srvHeap;
    DX12RootSignature       _rootSignature;
    DX12Renderer            _renderer;
    DX12UploadContext       _uploader;
    PSOManager              _psoManager;
	ShaderManager		    _shaderManager;
	TextureManager          _textureManager;

public:

	DX12Device& device() { return _device; }
    DX12Renderer& renderer() { return _renderer; }
    DX12CBVSRVUAVHeap& srvHeap() { return _srvHeap; }
    DX12UploadContext& uploader() { return _uploader; }
	ShaderManager& shaderManager() { return _shaderManager; }
	PSOManager& psoManager() { return _psoManager; }
	TextureManager& textureManager() { return _textureManager; }

    void create(HWND hwnd, int width, int height) {

        _device.create();

        _queues.create(_device.device());

        _rootSignature.create(_device.device());

        _swapchain.create(_queues.graphicsQueue(), hwnd, width, height, bufferCount);

        _targets.create(_device.device(), _swapchain, width, height);

        _srvHeap.create(_device.device(), srvHeapSize);

        _uploader.create(_device.device(), _queues.graphicsQueue());

        // load static mesh shaders
        _shaderManager.load(
            _device.device(),
            "staticMesh",
            "Src/Assets/Shaders/staticMesh_vs.hlsl",
            "Src/Assets/Shaders/staticMesh_ps.hlsl"
        );

        _psoManager.createPSO(
            _device.device(),
            _rootSignature.rootSignature(),
            "staticMeshPSO",
            _shaderManager.find("staticMesh")->vs.Get(),
            _shaderManager.find("staticMesh")->ps.Get(),
            DX12VertexLayoutCache::getStaticLayout()
        );

        _renderer.create(
            _device.device(),
            _queues.graphicsQueue(),
            _swapchain,
            _targets,
            _rootSignature,
            _srvHeap,
            _shaderManager,
            _psoManager,
            _textureManager,
            width,
            height
        );
    }

public:

	RenderContext() = default;
       
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&) = default;
    RenderContext& operator=(RenderContext&&) = default;
};
