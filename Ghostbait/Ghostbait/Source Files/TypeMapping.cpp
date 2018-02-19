#include "TypeMapping.h"
std::atomic_uint TypeMap::lastTypeID(1);
std::atomic_uint TypeMap::lastObjectTypeID(1);
std::atomic_uint TypeMap::lastComponentTypeID(1);
std::unordered_map<std::string, unsigned> TypeMap::mapname;
std::unordered_map<std::string, unsigned> TypeMap::mapnameObject;
std::unordered_map<unsigned, std::string> TypeMap::mapnameObjectReverse;
std::unordered_map<std::string, unsigned> TypeMap::mapnameComponent;
