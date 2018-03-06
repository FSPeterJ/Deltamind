#pragma once
#include <fstream>
#include <DirectXMath.h>

namespace Reader {
	static std::ifstream* defaultStream;

	static int ReadInt(std::ifstream& stream) {
		int value;
		stream.read((char*)&value, sizeof(int));
		return value;
	}
	static float ReadFloat(std::ifstream& stream) {
		float value;
		stream.read((char*)&value, sizeof(float));
		return value;
	}
	static float ReadDouble(std::ifstream& stream) {
		float value;
		stream.read((char*) &value, sizeof(double));
		return value;
	}
	static char ReadChar(std::ifstream& stream) {
		char value;
		stream.read(&value, sizeof(char));
		return value;
	}
	static std::string ReadString(std::ifstream& stream, int size) {
		char* value = new char[size];
		stream.read(value, size);
		std::string result(value);
		delete[] value;
		return result;
	}
	static DirectX::XMFLOAT4X4 ReadMatrix(std::ifstream& stream) {
		DirectX::XMFLOAT4X4 value;
		stream.read((char*)&value, sizeof(DirectX::XMFLOAT4X4));
		return value;
	}
	//UN-TESTED FUNCTION
	static void ReadBytes(std::ifstream& stream, int bytes, char** outData) {
		stream.read(*outData, bytes);
	}


	static int ReadInt() {
		return ReadInt(*defaultStream);
	}
	static float ReadFloat() {
		return ReadFloat(*defaultStream);
	}
	static float ReadDouble() {
		return ReadDouble(*defaultStream);
	}
	static char ReadChar() {
		return ReadChar(*defaultStream);
	}
	static std::string ReadString(int size) {
		return ReadString(*defaultStream, size);
	}
	static DirectX::XMFLOAT4X4 ReadMatrix() {
		return ReadMatrix(*defaultStream);
	}
	//UN-TESTED FUNCTION
	static void ReadBytes(int bytes, char** outData) {
		ReadBytes(*defaultStream, bytes, outData);
	}
}

namespace Writer {
	static std::ofstream* defaultStream;

	static void WriteInt(std::ofstream& stream, int val) {
		stream.write((char*)&val, sizeof(int));
	}
	static void WriteInt(std::ofstream& stream, size_t val) {
		int value = (int)val;
		WriteInt(stream, value);
	}
	static void WriteFloat(std::ofstream& stream, float val) {
		stream.write((char*)&val, sizeof(float));
	}

	static void WriteDouble(std::ofstream& stream, double val) {
		stream.write((char*) &val, sizeof(double));
	}
	static void WriteChar(std::ofstream& stream, char val) {
		stream.write(&val, sizeof(char));
	}
	static void WriteString(std::ofstream& stream, std::string val) {
		const char* value = val.c_str();
		stream.write(value, val.length() + 1);
	}
	static void WriteIntString(std::ofstream& stream, std::string val) {
		WriteInt(stream, val.length() + 1);
		WriteString(stream, val);
	}
	static void WriteMatrix(std::ofstream& stream, DirectX::XMFLOAT4X4& val) {
		stream.write((char*)&val, sizeof(DirectX::XMFLOAT4X4));
	}
	static void WriteBytes(std::ofstream& stream, const char* val, int bytes) {
		stream.write(val, bytes);
	}

	static void WriteInt(int val) {
		WriteInt(*defaultStream, val);
	}
	static void WriteInt(size_t val) {
		WriteInt(*defaultStream, val);
	}
	static void WriteFloat(float val) {
		WriteFloat(*defaultStream, val);
	}

	static void WriteDouble(double val) {
		WriteDouble(*defaultStream, val);
	}
	static void WriteChar(char val) {
		WriteChar(*defaultStream, val);
	}
	static void WriteString(std::string val) {
		WriteString(*defaultStream, val);
	}
	static void WriteIntString(std::string val) {
		WriteIntString(*defaultStream, val);
	}
	static void WriteMatrix(DirectX::XMFLOAT4X4& val) {
		WriteMatrix(*defaultStream, val);
	}
	static void WriteBytes(const char* val, int bytes) {
		WriteBytes(*defaultStream, val, bytes);
	}
}
