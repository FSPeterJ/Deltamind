#include "ObjectFactory.h"

std::unordered_map<int, std::function<Object*(void)>> ObjectFactory::registeredConstructors;

ObjectManager* ObjectFactory::objMan;

std::unordered_map<std::string, int> ObjectFactory::prefabNames;

std::vector<IComponentManager*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;