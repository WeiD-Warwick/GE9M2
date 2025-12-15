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

    float                   _aspectRatio = 0;
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
    float aspectRatio() { return _aspectRatio; }

    RenderContext(HWND hwnd, int width, int height) {

        _aspectRatio = (float)width / (float)height;

        _device.create();

        _queues.create(_device.dxDevice());

        _rootSignature.create(_device.dxDevice());

        _swapchain.create(_queues.graphicsQueue(), hwnd, width, height, bufferCount);

        _targets.create(_device.dxDevice(), _swapchain, width, height);

        _srvHeap.create(_device.dxDevice(), srvHeapSize);

        _uploader.create(_device.dxDevice(), _queues.graphicsQueue());

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

        registerResource();
    }

public:
       
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&) = default;
    RenderContext& operator=(RenderContext&&) = default;

private:
    void registerResource() {
        // load default texture
        _textureManager.loadTexture(
            _device.dxDevice(),
            _uploader,
            _srvHeap,
            "__default",
            "Src/Assets/Textures/__default.png");

        // load default texture
        _textureManager.loadTexture(
            _device.dxDevice(),
            _uploader,
            _srvHeap,
            "rgb_green",
            "Src/Assets/Textures/rgb_green.png");

        _textureManager.loadTexture(
            _device.dxDevice(),
            _uploader,
            _srvHeap,
            "skySphere",
            "Src/Assets/Textures/skySphere.png");

        // load static mesh shaders
        Shader* staticMeshShader = _shaderManager.load(
            _device.dxDevice(),
            "staticMeshShader",
            "Src/Assets/Shaders/staticMesh_vs.hlsl",
            "Src/Assets/Shaders/staticMesh_ps.hlsl"
        );

        Shader* animatedMeshShader = _shaderManager.load(
            _device.dxDevice(),
            "animatedMeshShader",
            "Src/Assets/Shaders/animatedMesh_vs.hlsl",
            "Src/Assets/Shaders/animatedMesh_ps.hlsl"
        );

        Shader* skySphereShader = _shaderManager.load(
            _device.dxDevice(),
            "skySphereShader",
            "Src/Assets/Shaders/skySphere_vs.hlsl",
            "Src/Assets/Shaders/skySphere_ps.hlsl"
        );

        Shader* debugShader = _shaderManager.load(
            _device.dxDevice(),
            "debugShader",
            "Src/Assets/Shaders/debug_vs.hlsl",
            "Src/Assets/Shaders/debug_ps.hlsl"
        );

        PSOParam staticPSOParam;
        staticPSOParam.psoName = "staticMeshPSO";
        staticPSOParam.psBlob = staticMeshShader->ps.Get();
        staticPSOParam.vsBlob = staticMeshShader->vs.Get();
        staticPSOParam.layout = DX12VertexLayoutCache::getStaticLayout();
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            staticPSOParam
        );

        PSOParam animatedPSOParam;
        animatedPSOParam.psoName = "animatedMeshPSO";
        animatedPSOParam.psBlob = animatedMeshShader->ps.Get();
        animatedPSOParam.vsBlob = animatedMeshShader->vs.Get();
        animatedPSOParam.layout = DX12VertexLayoutCache::getAnimatedLayout();
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            animatedPSOParam
        );

        PSOParam skySpherePSOParam;
        skySpherePSOParam.psoName = "skySpherePSO";
        skySpherePSOParam.psBlob = skySphereShader->ps.Get();
        skySpherePSOParam.vsBlob = skySphereShader->vs.Get();
        skySpherePSOParam.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        skySpherePSOParam.depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        skySpherePSOParam.layout = DX12VertexLayoutCache::getStaticLayout();
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            skySpherePSOParam
        );

        PSOParam FPSPSOParam;
        FPSPSOParam.psoName = "fpsPSO";
        FPSPSOParam.psBlob = animatedMeshShader->ps.Get();
        FPSPSOParam.vsBlob = animatedMeshShader->vs.Get();
        FPSPSOParam.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        FPSPSOParam.depthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        FPSPSOParam.layout = DX12VertexLayoutCache::getAnimatedLayout();
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            FPSPSOParam
        );

        PSOParam debugParam;
        debugParam.psoName = "debugLinePSO";
        debugParam.psBlob = debugShader->ps.Get();
        debugParam.vsBlob = debugShader->vs.Get();
        debugParam.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        debugParam.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        debugParam.layout = DX12VertexLayoutCache::getStaticLayout();
        debugParam.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            debugParam
        );
    }
};
