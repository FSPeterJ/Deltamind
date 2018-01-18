#pragma once
#include <unordered_map>
#include <array>
#include "StdHeader.h"

class Pool {

	template<typename BucketType>
	class Bucket {
		std::vector<BucketType*> active;
		std::vector<BucketType*> inactive; //Linked list with head/tail ptrs
		BucketType* items;
	public:
		Bucket(size_t containmentSize) : items(new BucketType[containmentSize]) {}

		BucketType* CreateFreeSpot() { return &items[active.size()]; }
	};

	std::unordered_map<std::string, void*> bucketList;
	size_t bucketSize;

	template<typename BucketType2>
	void CreateBucket() {
		bucketList[GetTypeName<BucketType2>()] = new Bucket<BucketType2>(bucketSize);
	}
public:
	Pool(size_t containmentSize, size_t typeCount) : bucketSize(containmentSize) {
		bucketList.reserve(typeCount);
	}

	template<typename BucketType2>
	BucketType2* GetSpot(std::string itemId) {

		auto element = bucketList.find(itemId);

		if(element != bucketList.end()) {
			//bucket exists
		} else {
			//create bucket
			CreateBucket<BucketType2>();
		}

		void* raw_bucket = bucketList[itemId];
		Bucket<BucketType2>* ourBucket = (Bucket<BucketType2>*)raw_bucket;

		BucketType2* free_spot = ourBucket->CreateFreeSpot();
		
		return free_spot;
	}

};
