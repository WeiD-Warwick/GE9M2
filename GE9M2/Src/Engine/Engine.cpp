#include "Engine.h"
#include "../Game/Game.h"

Engine::Engine(HWND hwnd, int width, int height, Game* game)
	:
	_hwnd(hwnd),
	_width(width),
	_height(height),
	_scene(this),
	_renderContext(hwnd, width, height),
	_loader(_renderContext),
	_game(game)
{
	if (_game) {
		_game->onInit(*this, _scene);

		SkyLight skylight;
		skylight.color = { 1, 1, 1 };
		skylight.intensity = 0.3;
		_scene.setSkyLight(skylight);

		PointLight light1;
		light1.position = { 0, 5, 0 };
		light1.color = { 255, 0, 0 };
		light1.range = 20;
		light1.intensity = 15;

		PointLight light2;
		light1.position = { 0, 10, 0 };
		light1.color = { 0, 0.5, 0 };
		light1.range = 20;
		light1.intensity = 15;

		_scene.addLight(light1);
		_scene.addLight(light2);
	}
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

ModelLoader& Engine::loader() {
	return _loader;
}
