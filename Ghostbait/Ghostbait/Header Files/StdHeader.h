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
