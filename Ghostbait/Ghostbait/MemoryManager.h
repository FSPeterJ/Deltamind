#pragma once
#include <stdlib.h>

class MemoryManager
{
	char* raw_data = nullptr;
	char* bookmark = nullptr;
	char* maxMemory = nullptr;
public:
	MemoryManager(int bytes = 536870912, int alignment = 64)
	{
		raw_data = (char*)_aligned_malloc(bytes, alignment);
		bookmark = raw_data;
		maxMemory = bytes + raw_data;
	};
	~MemoryManager()
	{
		_aligned_free(raw_data);
	};

	void* RequestMemory(const int count, const int size)
	{
		char* returnAddress = bookmark;
		bookmark += count * size;
		return returnAddress;
	}

	template<typename Type>
	void* RequestMemeory(const int count)
	{
		char* returnAddress = bookmark;
		bookmark += count * sizeof(Type);
		return returnAddress;
	}
};
