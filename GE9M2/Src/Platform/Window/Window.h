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

	void resetCursorToCenter() {
		ShowCursor(FALSE);
		POINT centerClient = { width / 2, height / 2 };
		POINT centerScreen = centerClient;
		ClientToScreen(hwnd, &centerScreen);
		SetCursorPos(centerScreen.x, centerScreen.y);

		lastMouseX = centerScreen.x;
		lastMouseY = centerScreen.y;
	}

	void calculateDeltaAndAbsolutePos(int x_screen, int y_screen, int x_client, int y_client) {
		mouseDeltaX = x_screen - lastMouseX;
		mouseDeltaY = y_screen - lastMouseY;

		mouseX = x_client;
		mouseY = y_client;
	}

public:
	HWND hwnd = nullptr;
	bool keys[256] = {};
	bool mouseButtons[3] = {};
	int mouseX = 0;
	int mouseY = 0;

	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int lastMouseX = 0;
	int lastMouseY = 0;

	int width;
	int height;


public:

	void create(int x, int y, int w, int h, const std::string& title, bool fullWindow) {
		width = w;
		height = h;
		hinstance = GetModuleHandle(NULL);

		// Register window class
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

		// Create the window
		int finalX = x;
		int finalY = y;
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		if (fullWindow) {
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
			style = WS_POPUP | WS_VISIBLE;
			finalX = 0;
			finalY = 0;
		}

		RECT wr = { 0, 0, width, height };
		AdjustWindowRect(&wr, style, FALSE);
		hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			wname.c_str(),
			wname.c_str(),
			style, 
			x, y,
			width, height,
			NULL, NULL,
			hinstance,
			this
		);


		// Initialize mouse position
		resetCursorToCenter();
		mouseDeltaX = 0;
		mouseDeltaY = 0;
		window = this;

		if (fullWindow) {
			SetForegroundWindow(hwnd);
		}
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

	void handleMouseMove(LPARAM lParam) {
		int x_client = WINDOW_GET_X_LPARAM(lParam);
		int y_client = WINDOW_GET_Y_LPARAM(lParam);
		POINT p = { x_client, y_client };
		ClientToScreen(hwnd, &p);
		calculateDeltaAndAbsolutePos(p.x, p.y, x_client, y_client);
		resetCursorToCenter();
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

	case WM_KEYDOWN:
		if (wParam < 256) window->keys[(UINT)wParam] = true;
		return 0;

	case WM_KEYUP:
		if (wParam < 256) window->keys[(UINT)wParam] = false;
		return 0;

	case WM_LBUTTONDOWN:
		window->mouseX = WINDOW_GET_X_LPARAM(lParam);
		window->mouseY = WINDOW_GET_Y_LPARAM(lParam);
		window->mouseButtons[0] = true;
		return 0;
	case WM_LBUTTONUP:
		window->mouseX = WINDOW_GET_X_LPARAM(lParam);
		window->mouseY = WINDOW_GET_Y_LPARAM(lParam);
		window->mouseButtons[0] = false;
		return 0;

	case WM_MOUSEMOVE:
		window->handleMouseMove(lParam);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}