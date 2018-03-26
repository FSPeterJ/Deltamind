#pragma once

#include <ostream> //required for koenig lookups
#include <time.h>
namespace Common {

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
#define FILE__GHOST (Console::file_formatter(__FILE__).c_str())
#define FUNC__GHOST __FUNCTION__
#define TIME__GHOST (Console::time_formatter(time(0)).c_str())

#define LOG_STAMP TIME__GHOST << "[" << FILE__GHOST << "-" << FUNC__GHOST << "-" << LINE__GHOST << "]" << ": "

#define VERBOSE FILE__GHOST << " at " << FUNC__GHOST << "(" << LINE__GHOST << ") "

	class Console {
		using Writer = std::ostream;
		static void* hConsole;
		Console() {}

		class OutputWriter;

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

		class Logger;

		class PrefixWriter;

		static OutputWriter outputStream;
		static PrefixWriter errorPrefix;
		static PrefixWriter warningPrefix;
		static PrefixWriter outErrorPrefix;
		static Logger logger;

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
		static WriteLiner	Log;

		static std::string file_formatter(char* source);
		static std::string time_formatter(const time_t time);

		///<summary>
		///Allocates memory for the Console.
		///<summary>
		static void Allocate();

		/// <summary>
		/// Minimizes the console.
		/// <summary>
		static inline void Minimize();

		/// <summary>
		/// Maximizes the console.
		/// <summary>
		static inline void Maximize();

		/// <summary>
		/// Frees the memory allocated by the console.
		/// <summary>
		static void Free();

		///<summary>
		///Sets the console color.
		///<summary>
		///<param name="color">The color.<param>
		static void SetColor(ConsoleColor color);
	};
}
