#pragma once
#include "../Platform/DX12/DX12Device.h"
#include "../Platform/DX12/DX12CommandQueue.h"
#include "../Platform/DX12/DX12Renderer.h"
#include "../Platform/DX12/DX12Upload.h"
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
    DX12Upload              _uploader;
    PSOManager              _psoManager;
	ShaderManager		    _shaderManager;
	TextureManager          _textureManager;

public:

	DX12Device& device() { return _device; }
    DX12Renderer& renderer() { return _renderer; }
    DX12CBVSRVUAVHeap& srvHeap() { return _srvHeap; }
    DX12Upload& uploader() { return _uploader; }
	ShaderManager& shaderManager() { return _shaderManager; }
	PSOManager& psoManager() { return _psoManager; }
	TextureManager& textureManager() { return _textureManager; }

    RenderContext(HWND hwnd, int width, int height) {

        _device.create();

        _queues.create(_device.dxDevice());

        _rootSignature.create(_device.dxDevice());

        _swapchain.create(_queues.graphicsQueue(), hwnd, width, height, bufferCount);

        _targets.create(_device.dxDevice(), _swapchain, width, height);

        _srvHeap.create(_device.dxDevice(), srvHeapSize);

        _uploader.create(_device.dxDevice(), _queues.graphicsQueue());

        // load static mesh shaders
        _shaderManager.load(
            _device.dxDevice(),
            "staticMeshShader",
            "Src/Assets/Shaders/staticMesh_vs.hlsl",
            "Src/Assets/Shaders/staticMesh_ps.hlsl"
        );

        _shaderManager.load(
            _device.dxDevice(),
            "animatedMeshShader",
            "Src/Assets/Shaders/animatedMesh_vs.hlsl",
            "Src/Assets/Shaders/animatedMesh_ps.hlsl"
        );

        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            "staticMeshPSO",
            _shaderManager.find("staticMeshShader")->vs.Get(),
            _shaderManager.find("staticMeshShader")->ps.Get(),
            DX12VertexLayoutCache::getStaticLayout()
        );

        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            "animatedMeshPSO",
            _shaderManager.find("animatedMeshShader")->vs.Get(),
            _shaderManager.find("animatedMeshShader")->ps.Get(),
            DX12VertexLayoutCache::getAnimatedLayout()
        );

        _renderer.create(
            _device.dxDevice(),
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
       
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&) = default;
    RenderContext& operator=(RenderContext&&) = default;
};
