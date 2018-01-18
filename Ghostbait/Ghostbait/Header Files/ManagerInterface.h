#pragma once
#include "ComponentBase.h"


class Object;

//Interface for systems to access managers
class ManagerInterface
{
public:
	ManagerInterface() {}
	virtual ~ManagerInterface() {}
	//
	virtual ComponentBase* GetElement(const unsigned int _id) = 0;
	//virtual void CloneComponent(Object* source, Object* destination) = 0;
};