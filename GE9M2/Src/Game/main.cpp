#include "../Engine/Platform/Window/Window.h"
#include "../Engine/Engine.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Window window;
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "MineCraft");

	Engine engine;
	engine.init(window.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	while (true) {
		window.processMessages();
		engine.beginFrame();
		

		engine.endFrame();
	};
	return 0;
};