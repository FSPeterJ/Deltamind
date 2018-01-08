#pragma once
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

#define MAX_LOADSTRING 100

class Window {

	HINSTANCE hInstance = nullptr;
	HWND mainWindow = nullptr;

	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING];
	UINT width = 800;
	UINT height = 100;

	/// <summary>
	///Registers the window class.
	/// </summary>
	/// <param name="hInstance">The window instance.</param>
	ATOM Register(HINSTANCE hInstance);

	/// <summary>
	/// Centers the window.
	/// </summary>
	/// <param name="wnd">The WND.</param>
	void CenterWindow(HWND wnd);
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="Window"/> class.
	/// </summary>
	/// <param name="_w">The width.</param>
	/// <param name="_h">The height.</param>
	Window(UINT _w, UINT _h);

	/// <summary>
	/// Saves instance handle and creates main window.
	/// In this function, we save the instance handle in a global variable and create and display the main program window.
	/// </summary>
	/// <returns>TRUE if initialization was successful, otherwise FALSE.</returns>
	BOOL Initialize(HINSTANCE, int);

	/// <summary>
	/// Processes messages for the main window.
	/// </summary>
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	inline UINT GetWidth() const { return width; }
	inline UINT GetHeight() const { return height; }
	inline HWND GetOutputWindow() const { return mainWindow; }
	inline LPWSTR GetTitle() { return szTitle; }
	inline LPWSTR GetWindowClass() { return szWindowClass; }
};
