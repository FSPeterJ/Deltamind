#pragma once
#include <Windows.h>

class Messagebox {
public:
	/// <summary>
	/// Displays a debug message box.
	/// </summary>
	/// <param name="title">The title.</param>
	/// <param name="msg">The message.</param>
	inline static void ShowDebug(LPCWSTR title, LPCWSTR msg) { MessageBox(0, msg, title, MB_ICONEXCLAMATION | MB_OK); }

	/// <summary>
	/// Displays an error message box.
	/// </summary>
	/// <param name="title">The title.</param>
	/// <param name="msg">The message.</param>
	inline static void ShowError(LPCWSTR title, LPCWSTR msg) { if(MessageBox(0, msg, title, MB_ICONERROR | MB_RETRYCANCEL) != IDRETRY) { abort(); } }
};
