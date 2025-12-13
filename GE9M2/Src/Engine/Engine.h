#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Scene/Scene.h"
#include "ModelLoader.h"
#include "Graphics/Mesh/MeshLib.h"
#include "Platform/Window/Window.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Material/Material.h"
#include "Scene/Components/StaticMeshRenderComponent.h"
#include "Scene/Components/PlayerControllerComponent.h"
#include "Scene/Components/SkySphereRenderComponent.h"

class Engine {
private:
	const int       bufferCount = 2;
	int             _width = 0;
	int             _height = 0;
	HWND            _hwnd = nullptr;
	Scene           _scene;
	RenderContext   _renderContext;

	ModelLoader _loader;
	MeshLibrary _meshLib;

public:

	Engine(HWND hwnd, int width, int height) 
		: 
		_hwnd(hwnd),
		_width(width),
		_height(height),
		_scene(this),
		_renderContext(hwnd, width, height),
		_loader(_renderContext),
		_meshLib(_renderContext)
	{
		initTexture();
		initMeshes();
		initScene();
	}

	~Engine() {}

	void beginFrame() {
		_renderContext.renderer().beginFrame();
	}

	void frame(float dt) {
		beginFrame();
		_scene.update(dt);
		_scene.render(_renderContext);
		endFrame();
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

	void initTexture() {
		_renderContext.textureManager().loadTexture(
			_renderContext.device().dxDevice(),
			_renderContext.uploader(),
			_renderContext.srvHeap(),
			"Src/Assets/Textures/SkySphere/SkySphere.png",
			"Src/Assets/Textures/SkySphere/SkySphere.png"
		);
	}

	void initMeshes() {
	}

	void initScene() {
		// Camera
		GameObject* player = _scene.createObject();
		//GameObject* player = _loader.generateGameObjectFrom("Src/Assets/Models/Uzi.gem", &_scene);
		CameraComponent* cam = player->addComponent<CameraComponent>((float)_width / (float)_height);
		player->addComponent<PlayerControllerComponent>();
		_scene.setMainCamera(cam);

		player->transform.position = Vec3(0, 10, -20);

		//GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/acacia_003.gem", &_scene);
		GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/TRex.gem", &_scene);
		model->transform.scale = Vec3(0.1f, 0.1f, 0.1f);
		model->transform.position = Vec3(0, 0, 0);

		createGround();
		createSkybox();
	}


	void createGround() {
		GameObject* ground = _scene.createObject();

		std::vector<DX12Mesh*> meshes = { &_meshLib.plane };
		std::vector<std::string> textures;

		ground->addComponent<StaticMeshRenderComponent>(meshes, textures);
		ground->transform.position = Vec3(0, 0, 0);
		ground->transform.scale = Vec3(500, 1, 500);

	}

	void createSkybox() {
		SkySphereMaterial* skyMaterial = new SkySphereMaterial("skySphereShader", "Src/Assets/Textures/SkySphere/SkySphere.png");
		GameObject* sky = _scene.createObject();
		sky->addComponent<SkySphereRenderComponent>(&_meshLib.skySphere, skyMaterial);

	}
};