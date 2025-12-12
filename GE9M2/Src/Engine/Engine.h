#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Scene/Scene.h"
#include "Platform/Window/Window.h"
#include "Graphics/RenderContext.h"
#include "Scene/Components/StaticMeshRenderComponent.h"
#include "ModelLoader.h"
#include "Scene/Components/PlayerControllerComponent.h"

class Engine {
private:
	const int       bufferCount = 2;
	int             _width = 0;
	int             _height = 0;
	HWND            _hwnd = nullptr;
	Scene           _scene;
	RenderContext   _renderContext;

	DX12Mesh _cubeMesh;
	ModelLoader _loader;

public:

	Engine(HWND hwnd, int width, int height) 
		: 
		_hwnd(hwnd),
		_width(width),
		_height(height),
		_scene(this),
		_renderContext(hwnd, width, height),
		_loader(_renderContext)
	{
		initMeshes();

		initScene();
	}

	~Engine() {}

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

	DX12Upload& uploader() {
		return _renderContext.uploader();
	}

	RenderContext& renderContext() {
		return _renderContext;
	}

	void initMeshes() {
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		// back
		vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));
		// front
		vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));
		// left
		vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
		// right
		vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
		// top
		vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));
		// bottom
		vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));


		std::vector<unsigned int> indices;
		// back
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		// front
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		// left
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		// right
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		// top
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		// bottom
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);


		_cubeMesh.createStatic(
			_renderContext.device().dxDevice(),
			_renderContext.uploader(),
			vertices,
			indices
		);
	}

	void initScene() {
		// Camera
		GameObject* player = _scene.createObject();
		CameraComponent* cam = player->addComponent<CameraComponent>((float)_width / (float)_height);
		player->addComponent<PlayerControllerComponent>();
		
		_scene.setMainCamera(cam);

		player->transform.position = Vec3(0, 10, -20);

		//GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/acacia_003.gem", &_scene);
		GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/TRex.gem", &_scene);
		model->transform.scale = Vec3(0.1f, 0.1f, 0.1f);
		model->transform.position = Vec3(0, 0, 0);
	}

};