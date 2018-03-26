#pragma once
#include <iosfwd>
#include <string>

namespace DirectX {
	struct XMFLOAT4X4;
}
namespace Common {
	class Reader {
		static std::ifstream* defaultStream;
		virtual void Abstract() = 0;
		static bool created;
	public:
		static void OpenStream(std::ifstream &s);

		static void CreateStream(const char* file);

		static void CloseStream();

		template <typename T> static T Read();
		template <typename T> static T Read(std::ifstream& stream);

		static int ReadInt(std::ifstream& stream);
		static float ReadFloat(std::ifstream& stream);
		static double ReadDouble(std::ifstream& stream);
		static char ReadChar(std::ifstream& stream);

		static std::string ReadString(std::ifstream& stream, int size);

		static std::string ReadStringWithSize();


		//UN-TESTED FUNCTION
		static void ReadBytes(std::ifstream& stream, int bytes, char** outData);

		static DirectX::XMFLOAT4X4 ReadMatrix();
		static DirectX::XMFLOAT4X4 ReadMatrix(std::ifstream& stream);

		static int ReadInt();
		static float ReadFloat();
		static double ReadDouble();
		static char ReadChar();

		static std::string ReadString(int size);
	};

	class Writer {
		static std::ofstream* defaultStream;
		virtual void Abstract() = 0;
		static bool created;
	public:
		static void OpenStream(std::ofstream &s);

		static void CreateStream(const char* file);

		static void CloseStream();

		template <typename T> static void Write(std::ofstream& stream, T val);
		template <typename T> static void Write(T val);

		static void WriteInt(std::ofstream& stream, int val);
		static void WriteInt(std::ofstream& stream, std::size_t val);
		static void WriteFloat(std::ofstream& stream, float val);
		static void WriteDouble(std::ofstream& stream, double val);
		static void WriteChar(std::ofstream& stream, char val);
		static void WriteString(std::ofstream& stream, std::string val);

		static void WriteStringWithSize(std::ofstream& stream, std::string val);

		static void WriteBytes(std::ofstream& stream, const char* val, int bytes);

		static void WriteInt(int val);
		static void WriteInt(std::size_t val);
		static void WriteFloat(float val);
		static void WriteDouble(double val);
		static void WriteChar(char val);

		static void WriteString(std::string val);
		static void WriteStringWithSize(std::string val);

		static void WriteMatrix(std::ofstream& stream, DirectX::XMFLOAT4X4& val);
		static void WriteMatrix(DirectX::XMFLOAT4X4& val);

		static void WriteBytes(const char* val, int bytes);
	};
}
