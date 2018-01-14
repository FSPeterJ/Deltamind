#include "Window.h"

Window::Window(UINT _w, UINT _h) {
	width = _w; height = _h;
	LoadStringW(hInstance, IDS_APP_TITLE, GetTitle(), MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GHOSTBAIT, GetWindowClass(), MAX_LOADSTRING);
	Register(hInstance);
}

void Window::CenterWindow(HWND wnd) {
	RECT rc;

	GetWindowRect(wnd, &rc);

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

	SetWindowPos(wnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	SetForegroundWindow(wnd);
}

BOOL Window::Initialize(HINSTANCE _hInstance, int nCmdShow) {
	hInstance = _hInstance;

	RECT windowSize = {0, 0, (LONG) width, (LONG) height};
	AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, FALSE);

	mainWindow = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowSize.right - windowSize.left,
		windowSize.bottom - windowSize.top,
		NULL, NULL, hInstance, nullptr);

	if(!mainWindow) { return FALSE; }

	CenterWindow(mainWindow);
	ShowWindow(mainWindow, nCmdShow);
	UpdateWindow(mainWindow);



	return TRUE;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_DESTROY: { PostQuitMessage(0); } break;

	case WM_MOUSEMOVE:
	{
		
	} break;

	case WM_KEYDOWN:
	{
		
	} break;

	case WM_KEYUP:
	{
		
	} break;

	default: return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}

ATOM Window::Register(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GHOSTBAIT));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_GHOSTBAIT);
	wcex.lpszClassName = szWindowClass;

	return RegisterClassExW(&wcex);
}
