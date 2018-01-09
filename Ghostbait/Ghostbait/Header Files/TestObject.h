#pragma once
#include "Object.h"

class TestObject:
	public Object
{
	int x = 133;
public:
	TestObject();
	~TestObject();
	//void testing() { Console::WriteLine(x); };

};

