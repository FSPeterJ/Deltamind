#pragma once

class Object;

// Allows for covariant return types in Manager interface
class ComponentBase
{
public:
	bool singleInstance = true;
	//If you are creating this base, you are doing something wrong.
};

class InstantiatedCompBase: public ComponentBase
{
public:
	InstantiatedCompBase()
	{
		singleInstance = false;
	}
	Object* parentObject = nullptr;
};