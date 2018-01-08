#pragma once
#include <windows.h>
#include <iostream>
#include <string>

namespace Console {
#ifndef NDEBUG
	/// <summary>
	/// Allocates memory for the Console.
	/// </summary>
	static void Allocate() {
		AllocConsole();
		FILE* new_std_in_out;
		freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
		freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
		//EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}

	inline static void Minimize() { ShowWindow(GetConsoleWindow(), SW_MINIMIZE); }

	inline static void Maximize() { ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); }

	/// <summary>
	/// Frees the memory allocated by the console.
	/// </summary>
	inline static void Free() { FreeConsole(); }

	inline static void Write(std::string s) { std::cout << s; }

	inline static void Write(int i) { std::cout << std::to_string(i); }

	inline static void Write(float i) { std::cout << std::to_string(i); }

	inline static void WriteLine(std::string s) { std::cout << s << std::endl; }

	inline static void WriteLine(int i) { std::cout << std::to_string(i) << std::endl; }

	inline static void WriteLine(float i) { std::cout << std::to_string(i) << std::endl; }
#else
	static void Allocate() {}
	static void Free() {}
	static void Write(std::string s) {}
	static void Write(int i) {}
	static void Write(float i) {}
	static void WriteLine(std::string s) {}
	static void WriteLine(int i) {}
	static void WriteLine(float i) {}
	static void Minimize() {}
	static void Maximize() {}
#endif
};
