#pragma once
#include "ComponentBase.h"


class Object;

//Interface for systems to access Managers
class ManagerInterface
{
public:
	ManagerInterface() {}
	virtual ~ManagerInterface() {}
	// 
	//virtual void CloneComponent(Object* source, Object* destination) = 0;
};