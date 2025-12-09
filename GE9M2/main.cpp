#include "Src/Engine/Platform/Window/Window.h"
#include "Src/Engine/Engine.h"
#include "Src/Engine/foundation/Timer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif

	Window window;
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "MineCraft");

	Engine engine(window.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	Timer timer;

	while (true) {
		window.processMessages();
		engine.frame(timer.dt());
	};
	return 0;
};