#pragma once
#include "ComponentBase.h"


class Object;



//IManager shouldn't exist on its own so make its ctor protected
class IManager
{
protected:
	IManager() {}
public:
	virtual ~IManager() {}
};


//Interface for systems to access managers
class IComponentManager : public IManager
{
public:
	//
	virtual ComponentBase* GetElement(const unsigned int _id) = 0;

	virtual ComponentBase* GetComponent(const char* filename) = 0;
	//virtual void CloneComponent(Object* source, Object* destination) = 0;
};
