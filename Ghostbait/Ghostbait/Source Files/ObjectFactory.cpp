#include "ObjectFactory.h"

std::unordered_map<unsigned, std::function<Object*(void)>> ObjectFactory::registeredConstructors;

ObjectManager* ObjectFactory::objMan;

std::unordered_map<std::string, unsigned> ObjectFactory::prefabNames;

std::unordered_map<unsigned, unsigned> ObjectFactory::Object2Prefab;

std::vector<IComponentManager*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;