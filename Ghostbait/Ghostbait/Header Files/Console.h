#pragma once

#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>

enum class ConsoleColor {
	LightGray = 7,
	LightGrey = LightGray,
	DarkGray,
	DarkGrey = DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	White = 15,
	Default = White,
};

#define LINE__GHOST __LINE__
#define FILE__GHOST (Console::file_formatter(__FILE__))
#define FUNC__GHOST __FUNCTION__

#define VERBOSE FILE__GHOST << " at " << FUNC__GHOST << "(" << LINE__GHOST << ") "

class Console {
	using Writer = std::ostream;
	static void* hConsole;
	Console() {}

	class OutputWriter: public std::streambuf {
	public:
		virtual int_type overflow(int_type c = EOF) {
			if(c != EOF) {
				TCHAR buf[] = {(TCHAR) c, '\0'};
				OutputDebugString(buf);
			}
			return c;
		}
	};

	class WriteLiner {
		std::streambuf* sbuf;

		class SuffixWriter: public std::ostream {
			bool need_newline = true;
			char suffix;
		public:
			SuffixWriter(std::streambuf* sbuf, char _suffix = '\n') : suffix(_suffix), std::ios(sbuf), std::ostream(sbuf) {}
			SuffixWriter(SuffixWriter&& other, char _suffix = '\n') : SuffixWriter(other.rdbuf(), _suffix) {
				other.need_newline = false;
			}
			~SuffixWriter() { this->need_newline && *this << suffix; }
		};
	public:
		WriteLiner(std::streambuf* sbuf) : sbuf(sbuf) {}
		template <typename T>
		SuffixWriter operator<< (T&& value) {
			SuffixWriter rc(sbuf); //temp obj to append
			rc << std::forward<T>(value);
			return rc;
		}
		SuffixWriter operator<< (std::ostream& (*manip)(std::ostream&)) {
			SuffixWriter rc(sbuf); //temp obj to append
			rc << manip;
			return rc;
		}
	};

	class PrefixWriter: public std::streambuf {
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

	static OutputWriter outputStream;
	static PrefixWriter errorPrefix;
	static PrefixWriter warningPrefix;
	static PrefixWriter outErrorPrefix;
public:
	static Writer		Write;
	static WriteLiner   WriteLine;
	static Writer		Error;
	static WriteLiner   ErrorLine;
	static Writer		Warning;
	static WriteLiner   WarningLine;
	static Writer		Out;
	static WriteLiner   OutLine;
	static Writer		ErrorOut;
	static WriteLiner   ErrorOutLine;

	static std::string file_formatter(char* source);

	/// <summary>
	/// Allocates memory for the Console.
	/// </summary>
	static void Allocate();

	/// <summary>
	/// Minimizes the console.
	/// </summary>
	static inline void Minimize();

	/// <summary>
	/// Maximizes the console.
	/// </summary>
	static inline void Maximize();

	/// <summary>
	/// Frees the memory allocated by the console.
	/// </summary>
	static void Free();

	/// <summary>
	/// Sets the console color.
	/// </summary>
	/// <param name="color">The color.</param>
	static void SetColor(ConsoleColor color);
};
