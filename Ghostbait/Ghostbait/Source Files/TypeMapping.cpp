#include "TypeMapping.h"
std::atomic_int TypeMap::lastTypeID(0);
std::unordered_map<std::string, int> TypeMap::mapname;