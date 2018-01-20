#include "MemoryManager.h"
#include <stdexcept>

void* MemoryManager::RequestMemory(const int count, const int size)
{
	char* returnAddress = bookmark;
	returnAddress += count * size + 1;
	if(returnAddress > maxMemory)
	{
		throw std::exception("FATAL ERROR: There is not enough memory allocated to initilize this resource!");
	}
	bookmark = returnAddress;
	return (void*)returnAddress;
}

