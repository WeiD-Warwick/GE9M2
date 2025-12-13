#include "Engine.h"
#include "Graphics/Material/Material.h"
#include "Foundation/Transform.h"
#include "Foundation/Maths.h"
#include "Scene/GameObject.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/AnimatedMeshRenderComponent.h"
#include "Scene/Components/StaticMeshRenderComponent.h"
#include "Scene/Components/SkySphereRenderComponent.h"
#include "Scene/Components/PlayerControllerComponent.h"

Engine::Engine(HWND hwnd, int width, int height)
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
	initScene();
}

Engine::~Engine() {}

void Engine::beginFrame() {
	_renderContext.renderer().beginFrame();
}

void Engine::frame(float dt) {
	beginFrame();
	_scene.update(dt);
	_scene.render(_renderContext);
	endFrame();
}

void Engine::endFrame() {
	_renderContext.renderer().endFrame();
}

void Engine::flush() {
	_renderContext.renderer().flushGraphicsQueue();
}

ID3D12GraphicsCommandList4* Engine::cmd() {
	return _renderContext.renderer().commandList();
}

DX12Upload& Engine::uploader() {
	return _renderContext.uploader();
}

RenderContext& Engine::renderContext() {
	return _renderContext;
}

void Engine::initTexture() {
	_renderContext.textureManager().loadTexture(
		_renderContext.device().dxDevice(),
		_renderContext.uploader(),
		_renderContext.srvHeap(),
		"Src/Assets/Textures/SkySphere/SkySphere.png",
		"Src/Assets/Textures/SkySphere/SkySphere.png"
	);
}

void Engine::initScene() {
	// Camera
	GameObject* player = _scene.createObject();
	//GameObject* player = _loader.generateGameObjectFrom("Src/Assets/Models/Uzi.gem", &_scene);
	CameraComponent* cam = player->addComponent<CameraComponent>((float)_width / (float)_height);
	player->addComponent<PlayerControllerComponent>();
	_scene.setMainCamera(cam);

	player->transform().position = Vec3(0, 10, -20);

	//GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/acacia_003.gem", &_scene);
	GameObject* model = _loader.generateGameObjectFrom("Src/Assets/Models/TRex.gem", &_scene);
	model->transform().scale = Vec3(0.1f, 0.1f, 0.1f);
	model->transform().position = Vec3(0, 0, 0);

	createGround();
	createSkybox();
}


void Engine::createGround() {
	GameObject* ground = _scene.createObject();

	std::vector<DX12Mesh*> meshes = { &_meshLib.plane };
	std::vector<std::string> textures;

	ground->addComponent<StaticMeshRenderComponent>(meshes, textures);
	ground->transform().position = Vec3(0, 0, 0);
	ground->transform().scale = Vec3(500, 1, 500);

}

void Engine::createSkybox() {
	SkySphereMaterial* skyMaterial = new SkySphereMaterial("skySphereShader", "Src/Assets/Textures/SkySphere/SkySphere.png");
	GameObject* sky = _scene.createObject();
	sky->addComponent<SkySphereRenderComponent>(&_meshLib.skySphere, skyMaterial);

}