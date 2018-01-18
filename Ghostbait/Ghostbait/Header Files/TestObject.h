#pragma once
#include "Object.h"

struct TestObject:
	public Object
{
	int x = 133;
public:
	TestObject();
	~TestObject();
	CloneFunction(TestObject)

};

