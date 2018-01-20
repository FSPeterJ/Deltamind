#pragma once
#include <stdlib.h>

class MemoryManager
{

	void *raw_data;
public:
	MemoryManager(int bytes = 536870912)
	{
		raw_data = malloc(bytes);
	};
	~MemoryManager() {};

};
