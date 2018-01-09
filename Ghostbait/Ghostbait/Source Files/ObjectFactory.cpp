#include "ObjectFactory.h"

std::unordered_map<std::string, std::function<Object*(void)>> ObjectFactory::m_RegisteredConstructors;

