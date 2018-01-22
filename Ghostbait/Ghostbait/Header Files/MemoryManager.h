#pragma once
#include <stdlib.h>

class MemoryManager
{
public:
	char* maxMemory = nullptr;
	char* bookmark = nullptr;
	char* raw_data = nullptr;
	//Roughly half a gig by default
	//The alignment is to hopefully not hit a cache page boundary
	MemoryManager(int bytes = 536870912, int alignment = 8)
	{
		raw_data = (char*)malloc(bytes);
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
		free(raw_data);
	};

	char* ReturnBuffer();

	char* RequestMemory(const int count, const int size);
};
