#pragma once
#include <cstdlib>
#include <fstream>


inline float FloatRandomRange(float a, float b) {
	return a + (float)rand() / (float)RAND_MAX * b - a;
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