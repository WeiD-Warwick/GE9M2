#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Scene/Scene.h"
#include "Platform/Window/Window.h"
#include "Graphics/Pipeline/RenderContext.h"
#include "Platform/DX12/DX12Mesh.h"
#include "Scene/Components/StaticMeshRenderComponent.h"

class Engine {
private:
    const int       bufferCount = 2;
    int             _width = 0;
    int             _height = 0;
    HWND            _hwnd = nullptr;
    Scene           _scene;
    RenderContext   _renderContext;

    DX12Mesh _planeMesh;

public:
    Engine(HWND hwnd, int width, int height) 
        : _hwnd(hwnd), _width(width), _height(height), _scene(this) {
        _renderContext.create(hwnd, width, height);

        initMeshes();

        initScene();
    }

    void beginFrame() {
        _renderContext.renderer().beginFrame();
    }

    void frame(float dt) {
        beginFrame();                               // DX12 prepare
        _scene.update(dt);                          // gameplay update
        _scene.render(_renderContext);              // components draw
        endFrame();                                 // DX12 present
    }

    void endFrame() {
        _renderContext.renderer().endFrame();
    }

    void flush() {
        _renderContext.renderer().flushGraphicsQueue();
    }

    ID3D12GraphicsCommandList4* cmd() {
        return _renderContext.renderer().commandList();
    }

    DX12UploadContext& uploader() {
        return _renderContext.uploader();
    }

    RenderContext& renderContext() {
        return _renderContext;
    }

    void initMeshes() {
        std::vector<STATIC_VERTEX> vertices;
        vertices.push_back(addVertex(Vec3(-0.5f, -0.5f, 0.0f), Vec3(0, 1, 0), 0, 0));
        vertices.push_back(addVertex(Vec3(0.5f, -0.5f, 0.0f), Vec3(0, 1, 0), 1, 0));
        vertices.push_back(addVertex(Vec3(-0.5f, 0.5f, 0.0f), Vec3(0, 1, 0), 0, 1));
        vertices.push_back(addVertex(Vec3(0.5f, 0.5f, 0.0f), Vec3(0, 1, 0), 1, 1));

        std::vector<unsigned int> indices;
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);


        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(2);

        _planeMesh.createStatic(
            _renderContext.device().device(),
            _renderContext.uploader(),
            vertices,
            indices
		);
    }

    void initScene() {
        // Camera
        GameObject* cameraObj = _scene.createObject();
        auto* cam = cameraObj->addComponent<CameraComponent>();
        _scene.setMainCamera(cam);

        cameraObj->transform.setPosition(Vec3(10, 100, 10));

        cam->setAspectRatio((float)_width, (float)_height);

        // Cube
        GameObject* plane = _scene.createObject();
        auto* r = plane->addComponent<StaticMeshRenderComponent>();
        r->setMesh(&_planeMesh);
        plane->transform.setPosition(Vec3(0, 0, 0));
    }

};