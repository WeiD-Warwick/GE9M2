#pragma once
#include <d3d12.h>
#include "../Game/Game.h"
#include "Platform/Window/Window.h"
#include "Graphics/RenderContext.h"
#include "ModelLoader.h"

class Engine {
private:
	const int       bufferCount = 2;
	int             _width = 0;
	int             _height = 0;
	HWND            _hwnd = nullptr;
	Scene           _scene;
	Game*			_game;
	RenderContext   _renderContext;
	ModelLoader		_loader;

public:

	bool			continueLoop = true;

	Engine(HWND hwnd, int width, int height, Game* game);

	~Engine();

	void beginFrame();

	void frame(float dt);

	void endFrame();

	void flush();

	ID3D12GraphicsCommandList4* cmd();

	DX12Upload& uploader();

	RenderContext& renderContext();

	MeshLibrary& meshLib();

	ModelLoader& loader();

	bool showCollisionBoxes = true;
};