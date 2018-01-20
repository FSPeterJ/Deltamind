#pragma once
#include <unordered_map>
#include <array>
#include "StdHeader.h"
#include "Console.h"
using namespace Console;
#include "Delegate.h"
#include <vector>

//#define BUCKET_SIZE 1
class Pool {
	Delegate<> Delete;

	template<typename BucketType>
	class Bucket {
		std::vector<BucketType*> activeList;
		std::vector<BucketType*> inactiveList; //Linked list with head/tail ptrs

		BucketType* items;

		//std::array<BucketType, BUCKET_SIZE> items_array;

		size_t item_count = 0;
		size_t size  = 0;

		void RemoveObjectFromActive(const BucketType* o) {
			auto it = std::find(activeList.begin(), activeList.end(), o);

			if(it != activeList.end()) {
				std::swap(*it, activeList.back());
				activeList.pop_back();
			}
		}

	public:
		Bucket(size_t containmentSize) : size(containmentSize), items(new BucketType[containmentSize]) {
			for(size_t i = 0; i < size; ++i) { items[i].~BucketType(); }
		}

		~Bucket() {
			delete[] items;
		}

		BucketType* GetItems() const { return items; }

		BucketType* CreateFreeSpot() { 
			if(item_count + 1 > size) 
				throw std::out_of_range("Pool size not sufficient."); 
			
			return &items[item_count++]; }

		BucketType* Activate() {
			if(inactiveList.size()) {

				//Debug("Activating some " << GetTypeName<BucketType>());

				activeList.push_back(inactiveList[0]);
				inactiveList.erase(inactiveList.begin());
				return activeList.back();
			}
		}

		void Deactivate(const BucketType* o) {

			//Debug("Deactivating a " << GetTypeName<BucketType>() << " : " << o);

			RemoveObjectFromActive(o);
			inactiveList.push_back(o);
		}
	};

	std::unordered_map<std::string, void*> bucketList;
	size_t bucketSize;

	template<typename BucketType2>
	void CreateBucket() {
		//Debug("Creating new bucket of type " << GetTypeName<BucketType2>().c_str());

		Bucket<BucketType2>* theBucket = new Bucket<BucketType2>(bucketSize);
		bucketList[GetTypeName<BucketType2>()] = theBucket;

		//Debug("Made bucket at " << bucketList[GetTypeName<BucketType2>()]);

		Delete.add([=]() { delete theBucket; });
	}
public:
	Pool(size_t _items_per_bucket, size_t prefabCount) : bucketSize(_items_per_bucket) {
	//	Debug("Creating new Pool");

		bucketList.reserve(prefabCount);
	}

	~Pool() {
		Delete();
		//for(auto b : bucketList) {
		//
		//	void* buck = b.second;
		//
		//	Bucket<void*>* buck_buck = (Bucket<void*>*)buck;
		//
		//	buck_buck->CLEAN();
		//
		//	//delete buck_buck;
		//}
	}

	const size_t BucketCount() const { return bucketList.size(); }

	/// <summary>
	/// Used to get a free spot in a bucket.
	/// </summary>
	/// <param name="itemId">The item identifier.</param>
	/// <returns>BucketType2 *.</returns>
	template<typename BucketType2>
	BucketType2* GetSpot(std::string itemId) {
		//Debug("Getting a new spot from a bucket of type " << GetTypeName<BucketType2>().c_str());


		auto element = bucketList.find(itemId);

		if(element != bucketList.end()) {
			//bucket exists
		//	Debug("Bucket exists");

		} else {
			//create bucket
			//Debug("Bucket does not exist");

			CreateBucket<BucketType2>();
		}

		void* raw_bucket = bucketList[itemId];
		Bucket<BucketType2>* ourBucket = (Bucket<BucketType2>*)raw_bucket;

		BucketType2* free_spot = ourBucket->CreateFreeSpot();
	//	Debug("Made a free spot in bucket : " << free_spot);

		return free_spot;
	}

	
	template<typename T>
	Bucket<T>* GetBucket() {
		std::string key = GetTypeName<T>();
		void* b = bucketList[key];
		return (Bucket<T>*)b;
	}





};
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
		elements = (T*)memManage->RequestMemory(pool_size, sizeof(T));
		//elements = new ((void*)(memManage->RequestMemory(pool_size, sizeof(T)))) T;
		for(int i = 0; i < pool_size; ++i)
		{
			new (&elements[i]) T;
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
