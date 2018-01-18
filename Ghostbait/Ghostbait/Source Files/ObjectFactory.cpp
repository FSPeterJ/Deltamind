#include "ObjectFactory.h"

std::unordered_map<int, std::function<Object*(void)>> ObjectFactory::registeredConstructors;


std::unordered_map<std::string, ObjectFactory::Prefab*> ObjectFactory::prefabNames;

std::vector<ManagerInterface*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;