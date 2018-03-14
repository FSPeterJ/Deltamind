#include "Console.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <corecrt_wtime.h>

std::string Console::file_formatter(char* source) {
	std::string src(source);
	size_t start = src.find_last_of('\\') + 1;
	return src.substr(start);
}
std::string Console::time_formatter(const time_t time) {
	tm dt;
	char buffer[30];
	localtime_s(&dt, &time);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", &dt);
	return std::string(buffer);
}

class Console::OutputWriter: public std::streambuf {
public:
	virtual int_type overflow(int_type c = EOF) {
		if(c != EOF) {
			TCHAR buf[] = {(TCHAR) c, '\0'};
			OutputDebugString(buf);
		}
		return c;
	}
};

class Console::PrefixWriter: public std::streambuf {
	std::string     prefix;
	bool            need_prefix = true;
	std::streambuf* sbuf;
	ConsoleColor color;

	int overflow(int c) {
		if(c == std::char_traits<char>::eof()) {
			return std::char_traits<char>::not_eof(c);
		}
		switch(c) {
		case '\n':
		case '\r':
			need_prefix = true;
			break;
		default:
			if(need_prefix) {
				Console::SetColor(color);
				this->sbuf->sputn(this->prefix.c_str(), this->prefix.size());
				need_prefix = false;
				Console::SetColor(ConsoleColor::Default);
			}
		}
		auto res = this->sbuf->sputc(c);
		return res;
	}
	int sync() {
		return this->sbuf->pubsync();
	}
public:
	PrefixWriter(std::string prefix, std::streambuf* sbuf, ConsoleColor _color) : prefix(std::move(prefix)), sbuf(sbuf), color(_color) {}
};

class Console::Logger : public std::streambuf {
	std::streambuf* sbuf;
	std::ofstream stream;

	int overflow(int c) {
		auto res = this->sbuf->sputc(c);
		stream << (char)c;
		return res;
	}
	int sync() {
		return this->sbuf->pubsync();
	}
public:
	Logger(const char* filePath, std::streambuf* sbuf) : sbuf(sbuf) {
		stream.open(filePath);
	}
	~Logger() {
		stream.close();
	}
};

Console::OutputWriter Console::outputStream;

Console::PrefixWriter Console::errorPrefix("ERROR: ", std::cout.rdbuf(), ConsoleColor::Red);
Console::PrefixWriter Console::warningPrefix("WARNING: ", std::cout.rdbuf(), ConsoleColor::Yellow);
Console::PrefixWriter Console::outErrorPrefix("ERROR: ", &outputStream, ConsoleColor::Red);
Console::Logger Console::logger("Log.txt", std::cout.rdbuf());



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
Console::WriteLiner Console::Log(&logger);


void* Console::hConsole;

void Console::Allocate() {
	AllocConsole();
	FILE* new_std_out;
	freopen_s(&new_std_out, "CONOUT$", "w", stdout);
	//freopen_s(&new_std_out, "CONOUT$", "w", stderr);
	//freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	std::ios_base::sync_with_stdio(false);
}

inline void Console::Minimize() { ShowWindow(GetConsoleWindow(), SW_MINIMIZE); }

inline void Console::Maximize() { ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); }

void Console::Free() { FreeConsole(); }

inline void Console::SetColor(ConsoleColor color) {
	SetConsoleTextAttribute(hConsole, (WORD) color);
}
