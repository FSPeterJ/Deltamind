#pragma once
#include <Windows.h>
#include <string>

class Messagebox {
public:
	/// <summary>
	/// Displays a debug message box.
	/// </summary>
	/// <param name="title">The title.</param>
	/// <param name="msg">The message.</param>
	inline static void ShowDebug(LPCWSTR title, std::wstring msg) { MessageBox(0, msg.c_str(), title, MB_ICONEXCLAMATION | MB_OK); }
	inline static void ShowDebug(LPCWSTR title, std::string msg) { ShowDebug(title, std::wstring(msg.begin(), msg.end())); }

	/// <summary>
	/// Displays an error message box.
	/// </summary>
	/// <param name="title">The title.</param>
	/// <param name="msg">The message.</param>
	inline static void ShowError(LPCWSTR title, std::wstring msg) { if(MessageBox(0, msg.c_str(), title, MB_ICONERROR | MB_RETRYCANCEL) != IDRETRY) { abort(); } }
	inline static void ShowError(LPCWSTR title, std::string msg) { ShowError(title, std::wstring(msg.begin(), msg.end())); }
};
