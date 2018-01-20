#pragma once
#include <vector>
#include "MemoryManager.h"
#include <new>


class _Pool_Base
{
protected:
	static MemoryManager* memManage;
public:
	static void RegisterMemory(MemoryManager* mem)
	{
		memManage = mem;
	}
};


template<typename T>
class Pool: _Pool_Base {


	std::vector< T*> activeList;
	std::vector< T*> inactiveList; //Linked list with head/tail ptrs
	T* elements;
	int pool_size;

	void RemoveObjectFromActive(const T* o) {
		auto it = std::find(activeList.begin(), activeList.end(), o);

		if(it != activeList.end()) {
			std::swap(*it, activeList.back());
			activeList.pop_back();
		}
	}

public:
	Pool(int size = 128)
	{
		pool_size = size;
		elements = new ((void*)(memManage->RequestMemory(pool_size, sizeof(T)))) T;
		for(int i = 0; i < pool_size; ++i)
		{
			inactiveList.push_back(&elements[i]);
		}
	}

	~Pool()
	{
		delete[] elements;
	}

	T &operator[](const int index)
	{
		return *activeList[index];
	}

	int GetActiveCount()
	{
		return activeList.size();
	}

	std::vector< T*>* GetActiveList()
	{
		return &activeList;
	}
	/// <summary>
	/// Activates the specified object.
	/// </summary>
	/// <param name="o">The object to activate.</param>
	/// <returns>A pointer to the activated or added Object.</returns>
	T* Activate() {
		if(inactiveList.size()) {
			activeList.push_back(inactiveList[0]);
			inactiveList.erase(inactiveList.begin());
			return activeList.back();
		}
		else {
			throw std::exception("Error: Attempted to activate more elements than the max pool size!");
			//GET READY TO CRASH :D
			return nullptr;
		}
	}
	/// <summary>
	/// Deactivates the specified object.
	/// </summary>
	/// <param name="o">The object to deactivate.</param>
	void Deactivate(T* o) {
		RemoveObjectFromActive(o);
		inactiveList.push_back(o);
	}
};
