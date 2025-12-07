#include "../Engine/Core/Window.h"
#include "../Engine/Core/Maths.h"
#include "../Engine/Core/Core.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Window window;
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "MineCraft");

	Core core;
	core.init(window.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	while (true) {

		core.beginFrame();
		window.processMessages();

		core.finishFrame();
	};
	core.flushGraphicsQueue();
	return 0;
};