#include "program.h"
#include "window.h"

HANDLE gh_instance;
HWND gh_main_wnd;
bool lockInstance();

Program g_program;

LRESULT CALLBACK WndProc(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show) {
	HRESULT hr = S_OK;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to initialize COM", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	if (lockInstance()) {
		MessageBox(nullptr, L"Another instance of the simulator is already running.", L"Error", MB_OK | MB_ICONERROR);
		CoUninitialize();
		return -1;
	}

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = h_instance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"TemplateWindowClass";
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
		CoUninitialize();
		return -1;
	}

	gh_main_wnd = CreateWindowEx(
		0,
		L"TemplateWindowClass",
		L"Template",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		h_instance,
		nullptr);

	if (gh_main_wnd == 0) {
		MessageBox(nullptr, L"Failed to create main window", L"Error", MB_OK | MB_ICONERROR);
		CoUninitialize();
		return -1;
	}

	ShowWindow(gh_main_wnd, n_cmd_show);
	UpdateWindow(gh_main_wnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	hr = g_program.initialize(gh_main_wnd);
	if (FAILED(hr)) {
		g_program.cleanup();
		if (gh_instance) {
			ReleaseMutex(gh_instance);
			CloseHandle(gh_instance);
		}
		MessageBox(nullptr, L"Failed to initialize program", L"Error", MB_OK | MB_ICONERROR);
		CoUninitialize();
		return -1;
	}

	bool running = true;
	while (running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		g_program.update();
		g_program.render();
	}
	g_program.cleanup();
	if (gh_instance) {
		ReleaseMutex(gh_instance);
		CloseHandle(gh_instance);
	}
	CoUninitialize();
	return static_cast<int>(msg.wParam);
}

bool lockInstance() {
	gh_instance = CreateMutex(nullptr, TRUE, L"DirectTemplateMutex");
	if (gh_instance == nullptr) {
		return false;
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return true;
	}
	return false;
}

LRESULT CALLBACK WndProc(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		ValidateRect(h_wnd, nullptr);
		return 0;
	default:
		return DefWindowProc(h_wnd, msg, w_param, l_param);
	}
}