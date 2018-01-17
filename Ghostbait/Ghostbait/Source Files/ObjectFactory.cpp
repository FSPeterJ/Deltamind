#include "ObjectFactory.h"

std::unordered_map<int, std::function<Object*(void)>> ObjectFactory::registeredConstructors;

MeshManager * ObjectFactory::meshManager;

std::unordered_map<std::string, ObjectFactory::Prefab*> ObjectFactory::prefabNames;

//std::unordered_map<int, Object*> ObjectFactory::prefabs;

std::vector<ObjectFactory::Prefab*> ObjectFactory::prefabs;