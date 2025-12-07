#pragma once
#define NOMINMAX
#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#include <Windows.h>
#include <string>

class Window;
Window* window;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Window {
private:
    HINSTANCE hinstance = nullptr;

public:
    HWND hwnd = nullptr;
    bool keys[256] = {};
    bool mouseButtons[3] = {};
    int mousex = 0;
    int mousey = 0;

public:

    void create(int x, int y, int width, int height, const std::string& title) {
		hinstance = GetModuleHandle(NULL);

		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		std::wstring wname = std::wstring(title.begin(), title.end());
		wc.lpszClassName = wname.c_str();
		wc.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&wc);

		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		hwnd = CreateWindowEx(WS_EX_APPWINDOW, wname.c_str(), wname.c_str(), style, x, y, width, height, NULL, NULL, hinstance, this);

		window = this;
	}

    // process all pending window messages.
	void processMessages() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    void updateMouse(int x, int y) {
		mousex = x;
		mousey = y;
	}
};

inline LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		exit(0);
		return 0;

	case WM_KEYUP:
		if (wParam < 256) window->keys[(unsigned int)wParam] = true;
		return 0;
	case WM_KEYDOWN:
		if (wParam < 256) window->keys[(unsigned int)wParam] = true;
		return 0;


	case WM_LBUTTONDOWN:
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
		window->mouseButtons[0] = true;
		return 0;
	case WM_LBUTTONUP:
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
		window->mouseButtons[0] = false;
		return 0;
	case WM_MOUSEMOVE:
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}