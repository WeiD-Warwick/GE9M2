#pragma once
#include <d3d12.h>

#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Platform/Window/Window.h"
#include "ModelLoader.h"
#include "Graphics/Mesh/MeshLib.h"

class Engine {
private:
	const int       bufferCount = 2;
	int             _width = 0;
	int             _height = 0;
	HWND            _hwnd = nullptr;
	Scene           _scene;
	RenderContext   _renderContext;
	ModelLoader		_loader;
	MeshLibrary		_meshLib;

public:

	Engine(HWND hwnd, int width, int height);

	~Engine();

	void beginFrame();

	void frame(float dt);

	void endFrame();

	void flush();

	ID3D12GraphicsCommandList4* cmd();

	DX12Upload& uploader();

	RenderContext& renderContext();

	void initTexture();
	void initMeshes();

	void initScene();


	void createGround();

	void createSkybox();
};