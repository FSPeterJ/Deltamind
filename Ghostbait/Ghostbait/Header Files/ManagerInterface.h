#pragma once
#include "ComponentBase.h"


//Interface for systems to access Managers
class ManagerInterface
{
public:
	ManagerInterface() {}
	virtual ~ManagerInterface() {}
	// 
	virtual ComponentBase* GetElement(const unsigned int _id) = 0;
	virtual int AddElement(const char* _meshFilePath) = 0;
};