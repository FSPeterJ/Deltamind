#pragma once
#include "ManagerInterface.h"

class ObjectFactoryInterface
{
public:
	ObjectFactoryInterface() {}
	virtual ~ObjectFactoryInterface() {}
	// Override with instructions for the factory on how to assemble the object
	//virtual void* GetMesh(ManagerInterface* manager, int id ) = 0;
	//virtual void* GetAnimation(ManagerInterface* manager, int id ) = 0;
	//virtual void* GetMaterial(ManagerInterface* manager, int id ) = 0;
	// no no no we need something more generic
	// W
};