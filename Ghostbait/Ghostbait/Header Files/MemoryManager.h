#pragma once
#include <stdlib.h>

class MemoryManager
{
	char* raw_data = nullptr;
	char* bookmark = nullptr;
	char* maxMemory = nullptr;
public:
	//Roughly half a gig by default
	//The alignment is to hopefully not hit a cache page boundary
	MemoryManager(int bytes = 536870912, int alignment = 8)
	{
		raw_data = (char*)_aligned_malloc(bytes, alignment);
		bookmark = raw_data;
		maxMemory = bytes + raw_data;
	};
	
	~MemoryManager()
	{
		//Dump ALL of the allocated memory
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//!!!								WARNING								!!!  
		//!!!	NO DESTRUCTORS WILL BE AUTO-CALLED THAT HAVE NOT BEEN CALLED	!!!  
		//!!!			YOU MUST CALL THEM YOURSELF BEFORE THIS RUNS			!!!
		//!!!								WARNING								!!!  
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		_aligned_free(raw_data);
	};

	void* RequestMemory(const int count, const int size);
};
