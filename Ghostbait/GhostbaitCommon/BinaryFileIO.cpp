#include "BinaryFileIO.h"
#include <DirectXMath.h>
#include <fstream>
#include <string>
namespace Common {

	std::ifstream* Reader::defaultStream;
	std::ofstream* Writer::defaultStream;
	bool Reader::created;
	bool Writer::created;

	void Reader::OpenStream(std::ifstream & s) {
		CloseStream();
		defaultStream = &s;
		created = false;
		//defaultStream->open(file, std::ios_base::binary);
	}

	void Reader::CreateStream(const char * file) {
		CloseStream();
		defaultStream = new std::ifstream(file, std::ios_base::binary);
		created = true;
	}

	void Reader::CloseStream() {
		if(defaultStream) { defaultStream->close(); if(created) { delete defaultStream; } defaultStream = nullptr; }
	}

	int Reader::ReadInt(std::ifstream & stream) { return Read<int>(stream); }

	float Reader::ReadFloat(std::ifstream & stream) { return Read<float>(stream); }

	double Reader::ReadDouble(std::ifstream & stream) { return Read<double>(stream); }

	char Reader::ReadChar(std::ifstream & stream) { { return Read<char>(stream); } }

	std::string Reader::ReadString(std::ifstream & stream, int size) {
		char* value = new char[size];
		stream.read(value, size);
		std::string result(value);
		delete[] value;
		return result;
	}

	std::string Reader::ReadStringWithSize() {
		return ReadString(ReadInt());
	}

	//UN-TESTED FUNCTION
	void Reader::ReadBytes(std::ifstream & stream, int bytes, char ** outData) { stream.read(*outData, bytes); }

	DirectX::XMFLOAT4X4 Reader::ReadMatrix() {
		return ReadMatrix(*defaultStream);
	}

	DirectX::XMFLOAT4X4 Reader::ReadMatrix(std::ifstream& stream) {
		DirectX::XMFLOAT4X4 value;
		stream.read((char*) &value, sizeof(DirectX::XMFLOAT4X4));
		return value;
	}

	int Reader::ReadInt() { return Read<int>(); }

	float Reader::ReadFloat() { return Read<float>(); }

	double Reader::ReadDouble() { return Read<double>(); }

	char Reader::ReadChar() { return Read<char>(); }

	std::string Reader::ReadString(int size) { return ReadString(*defaultStream, size); }

	void Writer::OpenStream(std::ofstream & s) {
		CloseStream();
		defaultStream = &s;
		created = false;
		//defaultStream->open(file, std::ios_base::binary);
	}

	void Writer::CreateStream(const char * file) {
		CloseStream();
		defaultStream = new std::ofstream(file, std::ios_base::binary);
		created = true;
	}

	void Writer::CloseStream() {
		if(defaultStream && defaultStream->is_open()) {
			defaultStream->close();
			if(created) { delete defaultStream; }
			defaultStream = nullptr;
		}
	}

	void Writer::WriteInt(std::ofstream & stream, int val) { Write(stream, val); }

	void Writer::WriteInt(std::ofstream & stream, std::size_t val) { Write(stream, (int) val); }

	void Writer::WriteFloat(std::ofstream & stream, float val) { Write(stream, val); }

	void Writer::WriteDouble(std::ofstream & stream, double val) { Write(stream, val); }

	void Writer::WriteChar(std::ofstream & stream, char val) { Write(stream, val); }

	void Writer::WriteString(std::ofstream & stream, std::string val) { stream.write(val.c_str(), val.length() + 1); }

	void Writer::WriteStringWithSize(std::ofstream & stream, std::string val) {
		WriteInt(stream, val.length() + 1);
		WriteString(stream, val);
	}

	void Writer::WriteBytes(std::ofstream & stream, const char * val, int bytes) { stream.write(val, bytes); }

	void Writer::WriteInt(int val) { Write(val); }

	void Writer::WriteInt(std::size_t val) { Write((int) val); }

	void Writer::WriteFloat(float val) { Write(val); }

	void Writer::WriteDouble(double val) { Write(val); }

	void Writer::WriteChar(char val) { Write(val); }

	void Writer::WriteString(std::string val) { WriteString(*defaultStream, val); }

	void Writer::WriteStringWithSize(std::string val) { WriteStringWithSize(*defaultStream, val); }

	void Writer::WriteMatrix(std::ofstream & stream, DirectX::XMFLOAT4X4 & val) {
		stream.write((char*) &val, sizeof(DirectX::XMFLOAT4X4));
	}

	void Writer::WriteMatrix(DirectX::XMFLOAT4X4 & val) {
		WriteMatrix(*defaultStream, val);
	}

	void Writer::WriteBytes(const char * val, int bytes) { WriteBytes(*defaultStream, val, bytes); }

	template<typename T>
	inline void Writer::Write(std::ofstream & stream, T val) { stream.write((char*) &val, sizeof(T)); }

	template<typename T>
	inline void Writer::Write(T val) { defaultStream->write((char*) &val, sizeof(T)); }

	template<typename T>
	inline T Reader::Read() { T val; defaultStream->read((char*) &val, sizeof(T)); return val; }

	template<typename T>
	inline T Reader::Read(std::ifstream & stream) { T val; stream.read((char*) &val, sizeof(T)); return val; }
}

template void Common::Writer::Write<int>(std::ofstream & stream, int val);
template void Common::Writer::Write<std::size_t>(std::ofstream & stream, std::size_t val);
template void Common::Writer::Write<double>(std::ofstream & stream, double val);
template void Common::Writer::Write<float>(std::ofstream & stream, float val);
template void Common::Writer::Write<char>(std::ofstream & stream, char val);
template void Common::Writer::Write<unsigned char>(std::ofstream & stream, unsigned char val);
template void Common::Writer::Write<DirectX::XMFLOAT4X4>(std::ofstream & stream, DirectX::XMFLOAT4X4 val);

template void Common::Writer::Write<int>(int val);
template void Common::Writer::Write<std::size_t>(std::size_t val);
template void Common::Writer::Write<double>(double val);
template void Common::Writer::Write<float>(float val);
template void Common::Writer::Write<char>(char val);
template void Common::Writer::Write<unsigned char>(unsigned char val);
template void Common::Writer::Write<DirectX::XMFLOAT4X4>(DirectX::XMFLOAT4X4 val);

template int Common::Reader::Read<int>(std::ifstream & stream);
template std::size_t Common::Reader::Read<std::size_t>(std::ifstream & stream);
template double Common::Reader::Read<double>(std::ifstream & stream);
template float Common::Reader::Read<float>(std::ifstream & stream);
template char Common::Reader::Read<char>(std::ifstream & stream);
template unsigned char Common::Reader::Read<unsigned char>(std::ifstream & stream);
template DirectX::XMFLOAT4X4 Common::Reader::Read<DirectX::XMFLOAT4X4>(std::ifstream & stream);

template int Common::Reader::Read<int>();
template std::size_t Common::Reader::Read<std::size_t>();
template double Common::Reader::Read<double>();
template float Common::Reader::Read<float>();
template char Common::Reader::Read<char>();
template unsigned char Common::Reader::Read<unsigned char>();
template DirectX::XMFLOAT4X4 Common::Reader::Read<DirectX::XMFLOAT4X4>();
