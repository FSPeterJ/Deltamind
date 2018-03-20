#pragma once
#include <fstream>
class Reader {
	static std::ifstream* defaultStream;
	virtual void Abstract() = 0;

public:
	static void OpenStream(const char* file, std::ifstream &s) {
		CloseStream();
		defaultStream = &s;
		//defaultStream->open(file, std::ios_base::binary);
	}

	static void CreateStream(const char* file) {
		CloseStream();
		defaultStream = new std::ifstream(file, std::ios_base::binary);
	}

	static void CloseStream() {
		if(defaultStream) { defaultStream->close(); delete defaultStream; defaultStream = nullptr; }
	}

	template <typename T> static T Read() { T val; defaultStream->read((char*) &val, sizeof(T)); return val; }
	template <typename T> static T Read(std::ifstream& stream) { T val; stream.read((char*) &val, sizeof(T)); return val; }

	static int ReadInt(std::ifstream& stream) { return Read<int>(stream); }
	static float ReadFloat(std::ifstream& stream) { return Read<float>(stream); }
	static double ReadDouble(std::ifstream& stream) { return Read<double>(stream); }
	static char ReadChar(std::ifstream& stream) { { return Read<char>(stream); } }

	static std::string ReadString(std::ifstream& stream, int size) {
		char* value = new char[size];
		stream.read(value, size);
		std::string result(value);
		delete[] value;
		return result;
	}

	//UN-TESTED FUNCTION
	static void ReadBytes(std::ifstream& stream, int bytes, char** outData) { stream.read(*outData, bytes); }

	static int ReadInt() { return Read<int>(); }
	static float ReadFloat() { return Read<float>(); }
	static double ReadDouble() { return Read<double>(); }
	static char ReadChar() { return Read<char>(); }

	static std::string ReadString(int size) { return ReadString(*defaultStream, size); }

	//UN-TESTED FUNCTION
	static void ReadBytes(int bytes, char** outData) { ReadBytes(*defaultStream, bytes, outData); }
};

class Writer {
	static std::ofstream* defaultStream;
	virtual void Abstract() = 0;
public:
	static void OpenStream(const char* file, std::ofstream &s) {
		CloseStream();
		defaultStream = &s;
		//defaultStream->open(file, std::ios_base::binary);
	}

	static void CreateStream(const char* file) {
		CloseStream();
		defaultStream = new std::ofstream(file, std::ios_base::binary);
	}

	static void CloseStream() {
		if(defaultStream && defaultStream->is_open()) {
			defaultStream->close();
			delete defaultStream;
			defaultStream = nullptr;
		}
	}

	template <typename T> static void Write(std::ofstream& stream, T val) { stream.write((char*) &val, sizeof(T)); }

	template <typename T> static void Write(T val) { defaultStream->write((char*) &val, sizeof(T)); }

	static void WriteInt(std::ofstream& stream, int val) { Write(stream, val); }
	static void WriteInt(std::ofstream& stream, std::size_t val) { Write(stream, val); }
	static void WriteFloat(std::ofstream& stream, float val) { Write(stream, val); }
	static void WriteDouble(std::ofstream& stream, double val) { Write(stream, val); }
	static void WriteChar(std::ofstream& stream, char val) { Write(stream, val); }
	static void WriteString(std::ofstream& stream, std::string val) { stream.write(val.c_str(), val.length() + 1); }

	static void WriteStringWithSize(std::ofstream& stream, std::string val) {
		WriteInt(stream, val.length() + 1);
		WriteString(stream, val);
	}

	static void WriteBytes(std::ofstream& stream, const char* val, int bytes) { stream.write(val, bytes); }

	static void WriteInt(int val) { Write(val); }
	static void WriteInt(std::size_t val) { Write(val); }
	static void WriteFloat(float val) { Write(val); }
	static void WriteDouble(double val) { Write(val); }
	static void WriteChar(char val) { Write(val); }

	static void WriteString(std::string val) { WriteString(*defaultStream, val); }
	static void WriteStringWithSize(std::string val) { WriteStringWithSize(*defaultStream, val); }

	static void WriteBytes(const char* val, int bytes) { WriteBytes(*defaultStream, val, bytes); }
};
