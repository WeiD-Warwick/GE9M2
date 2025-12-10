#include "Engine/Platform/Window/Window.h"
#include "Engine/Engine.h"
#include "Engine/foundation/Timer.h"

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
	window.create(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, "GE9M2", true);

	Engine engine(window.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	Timer timer;

	while (true) {
		window.processMessages();
		engine.frame(timer.dt());
	};
	return 0;
};