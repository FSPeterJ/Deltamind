#pragma once
#include <unordered_map>
#include <array>
#include "StdHeader.h"

class Pool {
	template<typename BucketType>
	class Bucket {
		std::vector<BucketType*> activeList;
		std::vector<BucketType*> inactiveList; //Linked list with head/tail ptrs
		BucketType* items;
		size_t item_count = 0;

		void RemoveObjectFromActive(const BucketType* o) {
			auto it = std::find(activeList.begin(), activeList.end(), o);

			if(it != activeList.end()) {
				std::swap(*it, activeList.back());
				activeList.pop_back();
			}
		}

	public:
		Bucket(size_t containmentSize) : items(new BucketType[containmentSize]) {}

		BucketType* CreateFreeSpot() { ++item_count; return &items[activeList.size()]; }

		BucketType* Activate() {
			if(inactiveList.size()) {
				activeList.push_back(inactiveList[0]);
				inactiveList.erase(inactiveList.begin());
				return activeList.back();
			}
		}

		void Deactivate(const BucketType* o) {
			RemoveObjectFromActive(o);
			inactiveList.push_back(o);
		}
	};

	std::unordered_map<std::string, void*> bucketList;
	size_t bucketSize;

	template<typename BucketType2>
	void CreateBucket() {
		bucketList[GetTypeName<BucketType2>()] = new Bucket<BucketType2>(bucketSize);
	}
public:
	Pool(size_t _bucketSize, size_t prefabCount) : bucketSize(_bucketSize) {
		bucketList.reserve(prefabCount);
	}

	const size_t BucketCount() const { return bucketList.size(); }

	/// <summary>
	/// Used to get a free spot in a bucket.
	/// </summary>
	/// <param name="itemId">The item identifier.</param>
	/// <returns>BucketType2 *.</returns>
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
