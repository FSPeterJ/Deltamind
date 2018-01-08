#pragma once
#include "Object.h"
#include <unordered_map>

class ObjectFactory
{

	template <typename T>
	static Object* ConstructorFunc()
	{
		return new T;
	}
	ObjectFactory() {};

	static std::unordered_map<std::string, std::function<Object*(void)>> m_RegisteredConstructors;

public:
	//static ObjectFactory * ObjectFactory::Instance()
	//{
	//	static ObjectFactory factory;
	//	return &factory;
	//}

	~ObjectFactory() {};
	template <typename T>
	static void Register(const char* name)
	{
		m_RegisteredConstructors[name] = &ConstructorFunc<T>;
	}
	static Object* CreateObject(char *type)
	{
		Object *newObject = NULL;

		newObject = m_RegisteredConstructors[type]();

		return newObject;
	}


};


