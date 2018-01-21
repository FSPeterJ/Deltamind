#include "MemoryManager.h"
#include <stdexcept>

char* MemoryManager::ReturnBuffer()
{
	return bookmark;
}

char* MemoryManager::RequestMemory(const int count, const int size)
{
	char* addressOffset = bookmark+ count * size;
	char* returnAddress = bookmark;
	if(addressOffset > maxMemory)
	{
		throw std::exception("FATAL ERROR: There is not enough memory allocated to initilize this resource!");
		return nullptr;
	}
	bookmark = bookmark + count * size;
	return returnAddress;
}

