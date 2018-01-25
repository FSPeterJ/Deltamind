#include "TypeMapping.h"
std::atomic_uint TypeMap::lastTypeID(0);
std::atomic_uint TypeMap::lastObjectTypeID(0);
std::atomic_uint TypeMap::lastComponentTypeID(0);
std::unordered_map<std::string, unsigned> TypeMap::mapname;
std::unordered_map<std::string, unsigned> TypeMap::mapnameObject;
std::unordered_map<std::string, unsigned> TypeMap::mapnameComponent;