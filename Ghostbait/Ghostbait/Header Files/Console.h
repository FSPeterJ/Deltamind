#pragma once
#include <windows.h>
#include <iostream>
#include <string>

namespace Console {
#ifndef NDEBUG
	/// <summary>
	/// Allocates memory for the Console.
	/// </summary>
	static void ConsoleAllocate() {
		AllocConsole();
		FILE* new_std_in_out;
		freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
		freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
		EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}

	inline static void Minimize() { ShowWindow(GetConsoleWindow(), SW_MINIMIZE); }

	inline static void Maximize() { ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); }

	/// <summary>
	/// Frees the memory allocated by the console.
	/// </summary>
	inline static void Free() { FreeConsole(); }

	inline static void Write(const std::string s) { std::cout << s; }

	inline static void Write(const int i) { std::cout << std::to_string(i); }

	inline static void Write(const float i) { std::cout << std::to_string(i); }

	inline static void WriteLine(const std::string s) { std::cout << s << std::endl; }

	inline static void WriteLine(const int i) { std::cout << std::to_string(i) << std::endl; }

	inline static void WriteLine(const float i) { std::cout << std::to_string(i) << std::endl; }

#define WriteT( s )   std::cout << s;

#define Debug( s ) do{  std::wostringstream os_; os_ << s << std::endl;  OutputDebugStringW( os_.str().c_str() ); }while(0)

#else
	static void ConsoleAllocate() {}
	static void Free() {}
	static void Write(const std::string s) {}
	static void Write(const int i) {}
	static void Write(const float i) {}
	static void WriteLine(const std::string s) {}
	static void WriteLine(const int i) {}
	static void WriteLine(const float i) {}
	static void Minimize() {}
	static void Maximize() {}
#endif
};
