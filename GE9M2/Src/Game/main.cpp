#include "../Engine/Core/Window.h"
#include "../Engine/Core/Maths.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Window window;
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "MineCraft");

	while (true) {
		window.processMessages();

			
	};
	return 0;
};