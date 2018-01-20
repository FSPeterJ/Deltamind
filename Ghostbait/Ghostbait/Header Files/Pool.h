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

		//BucketType* items;
		std::vector<BucketType> itemList;

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
		Bucket(size_t containmentSize) : size(containmentSize)/*, items(new BucketType[containmentSize])*/ {
			itemList = std::vector<BucketType>(containmentSize);
		}

		//void CLEAN() {
		//	delete[] items;
		//}
		//
		~Bucket() {
			for(size_t i = 0; i < size; ++i) {
				itemList[i].~BucketType();
			//	((BucketType) items[i]).~BucketType();
			}


			//delete[] items;
		}

		BucketType* GetItems() const { return items; }

		
		BucketType* CreateFreeSpot() { 
			
			itemList[item_count].~BucketType();
			return &itemList[item_count++];
		
		}

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

		Delete.add([=]() {
			delete theBucket;
		});
	}
public:
	Pool(size_t _bucketSize, size_t prefabCount) : bucketSize(_bucketSize) {
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
