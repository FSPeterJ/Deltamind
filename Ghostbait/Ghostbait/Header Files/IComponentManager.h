#pragma once
#include "ComponentBase.h"


class Object;




class IManager
{
public:
	IManager() {}
	virtual ~IManager() {}
};


//Interface for systems to access managers
class IComponentManager : IManager
{
public:
	//
	virtual ComponentBase* GetElement(const unsigned int _id) = 0;

	virtual ComponentBase* GetComponent(const char* filename) = 0;
	//virtual void CloneComponent(Object* source, Object* destination) = 0;
};
