#pragma once
#include "Object.h"

class TestObject:
	public Object
{
	int x = 133;
public:
	TestObject();
	~TestObject();
	static int GetTypeId() { return 1; };
};

