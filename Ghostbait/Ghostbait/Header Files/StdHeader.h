#pragma once
typedef unsigned PrefabId;

template<typename T> static std::string GetTypeName();

template<typename T>
std::string GetTypeName() {
	std::stringstream ss(typeid(T).name());
	std::string res;
	ss >> res;
	ss >> res;
	return res;
}
//;D
typedef int GhostInt;
typedef float GhostFloat;
typedef long GhostLong;
typedef unsigned GhostUInt;
typedef double GhostDouble;
typedef char GhostChar;
typedef short GhostShort;
typedef unsigned short GhostUShort;
typedef unsigned long GhostULong;
typedef bool GhostBool;
