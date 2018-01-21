//#pragma once
//#include <unordered_map>
//#include <array>
//#include "StdHeader.h"
//#include "Console.h"
//
//#include <vector>
//
////#define BUCKET_SIZE 1

#pragma once
#include <vector>
#include "MemoryManager.h"
#include <new>


//template<typename BucketType>
//class Bucket {
//	std::vector<BucketType*> activeList;
//	std::vector<BucketType*> inactiveList; //Linked list with head/tail ptrs
//
//	BucketType* items;
//
//	//std::array<BucketType, BUCKET_SIZE> items_array;
//
//	size_t item_count = 0;
//	size_t size = 0;
//public:
//	Bucket(size_t containmentSize): size(containmentSize), items(new BucketType[containmentSize]) {
//		for(size_t i = 0; i < size; ++i)
//		{
//			items[i].~BucketType();
//		}
//	}
//};


class _Pool_Base
{
protected:
	static MemoryManager* memManage;
	std::vector<void*> activeList;
	std::vector<void*> inactiveList; //Linked list with head/tail ptrs
public:
	static void RegisterMemory(MemoryManager* mem)
	{
		memManage = mem;
	}

	void RemoveObjectFromActive(const void* o) {
		auto it = std::find(activeList.begin(), activeList.end(), o);

		if(it != activeList.end()) {
			std::swap(*it, activeList.back());
			activeList.pop_back();
		}
	}

	void Deactivate(void* o) {
		RemoveObjectFromActive(o);
		inactiveList.push_back(o);
	}

	void* Activate() {
		if(inactiveList.size()) {
			activeList.push_back(inactiveList[0]);
			inactiveList.erase(inactiveList.begin());
			return activeList.back();
		}
		else {
			throw std::out_of_range("Pool is not of sufficient size.");
			//GET READY TO CRASH :D
			return nullptr;
		}
	}
};


template<typename T>
class Pool: public _Pool_Base {
	T* elements;
	size_t pool_size;

public:
	Pool(size_t size = 128)
	{
		pool_size = size;
		elements = (T*)memManage->RequestMemory(pool_size, sizeof(T));
		//elements = new ((void*)(memManage->RequestMemory(pool_size, sizeof(T)))) T;
		for(size_t i = 0; i < pool_size; ++i)
		{
			new (&elements[i]) T;
			inactiveList.push_back(&elements[i]);
		}
	}

	~Pool()
	{
		for (int i = 0; i < pool_size; ++i)
		{
			elements[i].~T();
		}
	}

	T &operator[](const size_t index)
	{
		return *(T*)activeList[index];
	}

	T* GetItems() const
	{
		return elements;
	}
	T* Activate() {
		return (T*)_Pool_Base::Activate();
	}

	size_t GetActiveCount()
	{
		return activeList.size();
	}

	std::vector< T*>* GetActiveList()
	{
		return (std::vector<T*>*)&activeList;
	}
	/// <summary>
	/// Activates the specified object.
	/// </summary>
	/// <param name="o">The object to activate.</param>
	/// <returns>A pointer to the activated or added Object.</returns>
	
};
