#pragma once
#include <fstream>
#include <random>

static std::default_random_engine generator;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

inline float FloatRandomRange(float a, float b) {
	return std::uniform_real_distribution<float> {a, b}(generator);
}

inline int LoadShaderFromCSO(char ** szByteCode, size_t & szByteCodeSize, const char * szFileName) {
	std::ifstream load;
	load.open(szFileName, std::ios_base::binary);
	if(!load.is_open()) return false;
	load.seekg(0, std::ios_base::end);
	szByteCodeSize = size_t(load.tellg());
	*szByteCode = new char[szByteCodeSize];
	load.seekg(0, std::ios_base::beg);
	load.read(*szByteCode, szByteCodeSize);
	load.close();
	return true;
}