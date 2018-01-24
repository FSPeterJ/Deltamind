#pragma once
#include <stdlib.h>

//Really dumb / simple memory allocator
class MemoryManager
{
public:
	char* maxMemory = nullptr;
	char* bookmark = nullptr;
	char* raw_data = nullptr;
	//Roughly half a gig by default
	//The alignment is to hopefully not hit a cache page boundary
	MemoryManager(int bytes = 536870912, int alignment = 64)
	{

		//raw_data = new char[bytes];
		raw_data = (char*)_aligned_malloc(bytes, alignment);
		bookmark = raw_data;
		maxMemory = bytes + raw_data;
	};
	
	~MemoryManager()
	{
		//Dump ALL of the allocated memory
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!								WARNING							 !!!  
		//!!!	NO DESTRUCTORS WILL BE AUTO-CALLED THAT HAVE NOT BEEN CALLED	 !!!  
		//!!!			YOU MUST CALL THEM YOURSELF BEFORE THIS RUNS			 !!!
		//!!!								WARNING							 !!!  
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		_aligned_free(raw_data);
		//delete[] raw_data;
	};

	//char* ReturnBuffer();

	char * RequestMemory(unsigned count, size_t size);
};
