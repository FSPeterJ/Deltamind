#include "TypeMapping.h"
std::atomic_int TypeMap::lastTypeID(0);
std::atomic_int TypeMap::lastObjectTypeID(0);
std::atomic_int TypeMap::lastComponentTypeID(0);
std::unordered_map<std::string, int> TypeMap::mapname;
std::unordered_map<std::string, int> TypeMap::mapnameObject;
std::unordered_map<std::string, int> TypeMap::mapnameComponent;