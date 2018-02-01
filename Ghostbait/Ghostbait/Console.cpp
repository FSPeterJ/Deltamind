#include "Console.h"

std::string Console::file_formatter(char* source) {
	std::string src(source);
	size_t start = src.find_last_of('\\') + 1;
	return src.substr(start);
}

Console::OutputWriter Console::outputStream;

Console::PrefixWriter Console::errorPrefix("ERROR: ", std::cout.rdbuf(), ConsoleColor::Red);
Console::PrefixWriter Console::warningPrefix("WARNING: ", std::cout.rdbuf(), ConsoleColor::Yellow);
Console::PrefixWriter Console::outErrorPrefix("ERROR: ", &outputStream, ConsoleColor::Red);

Console::Writer Console::Write(std::cout.rdbuf());
Console::Writer Console::Error(&errorPrefix);
Console::Writer Console::Warning(&warningPrefix);
Console::Writer Console::Out(&outputStream);
Console::Writer Console::ErrorOut(&outErrorPrefix);

Console::WriteLiner	Console::WriteLine(std::cout.rdbuf());
Console::WriteLiner	Console::ErrorLine(&errorPrefix);
Console::WriteLiner	Console::WarningLine(&warningPrefix);
Console::WriteLiner	Console::OutLine(&outputStream);
Console::WriteLiner Console::ErrorOutLine(&outErrorPrefix);

void* Console::hConsole;

void Console::Allocate() {
	AllocConsole();
	FILE* new_std_out;
	freopen_s(&new_std_out, "CONOUT$", "w", stdout);
	//freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

inline void Console::Minimize() { ShowWindow(GetConsoleWindow(), SW_MINIMIZE); }

inline void Console::Maximize() { ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); }

void Console::Free() { FreeConsole(); }

inline void Console::SetColor(ConsoleColor color) {
	SetConsoleTextAttribute(hConsole, (WORD) color);
}
