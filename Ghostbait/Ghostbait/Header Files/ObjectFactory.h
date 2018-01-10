#pragma once
#include "Object.h"
#include <unordered_map>
#include "MeshManager.h"

class ObjectFactory
{

	template <typename T>
	static Object* ConstructorFunc()
	{
		return new T;
	}
	ObjectFactory() {};

	static std::unordered_map<std::string, std::function<Object*(void)>> registeredConstructors;

	static std::unordered_map<std::string, Object*> prefabNames;
	// the state the object is in when it is first loaded
	static std::unordered_map<int, Object*> prefabs;
	// Managers
	static MeshManager * meshManager;
public:
	// Singleton?
	//static ObjectFactory * ObjectFactory::Instance()
	//{
	//	static ObjectFactory factory;
	//	return &factory;
	//}

	// Initializes the Object Factory and hands off the managers it needs to access
	static void Initialize(MeshManager * _meshManager)
	{
		meshManager = _meshManager;
	}

	~ObjectFactory() {};

	template <typename T>
	static void Register(const char* _name)
	{
		registeredConstructors[_name] = &ConstructorFunc<T>;
	}

	static Object* CreateObject(char *_type)
	{
		if(prefabNames[_type])
		{
			//Objectmanagers, turn on object
			Object *newObject = nullptr;
		}
		else
		{
			Object *newObject = nullptr;
			newObject = registeredConstructors[_type]();
			newObject->SetComponent(MESH, meshManager->GetElement(UINT_MAX));
			return newObject;
		}
	}

	static Object* CreateObject(int _id)
	{
		return nullptr;
	}


};


