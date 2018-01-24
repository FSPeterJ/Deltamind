#pragma once
#include "IComponentManager.h"
#include "Object.h"
#include "MessageEvents.h"
#include "Pool.h"
#define MAX_ENTITY_COUNT 16384


class ObjectManager: public IManager {
	Delegate<> Delete;
	MemoryManager* memMan;

	std::unordered_map<std::string, _Pool_Base*> objectpool;

	std::unordered_map<Object*,_Pool_Base*> pointers2Bucket;
	_Pool_Base* poolList;

	size_t poolListCount;
	size_t poolListNewIndex = 0;

	void Destroy(EventMessageBase* e);

public:
	void Update();

	template<typename PoolType>
	void CreatePool() {
		int typeID = TypeMap::getTypeId<PoolType>();
		if(typeID < poolListCount) {
			// HATE HATE HATE
			Pool<PoolType>* data = new ((char*) poolList + (sizeof(Pool<size_t>) * typeID)) Pool<PoolType>(128);
			Delete += [data]() { data->~Pool<PoolType>(); };
		} else {
			throw std::exception("Attempted to allocate a pool at an index larger than the maximum ObjectPool collection size.");
		}
	}
	ObjectManager(MemoryManager* _memMan);
	~ObjectManager();
	Object* Instantiate(int typeID);
	void Initialize(size_t prefabCount);
	void Shutdown() const;
	void CreatePool(int _size, Object* poolType) {}

	/// <summary>
	/// Used to get a free spot in a bucket.
	/// </summary>
	/// <param name="itemId">The item identifier.</param>
	/// <returns>BucketType2 *.</returns>
	//template<typename BucketType2>
	//BucketType2* GetSpot(std::string itemId) {
	//	//Debug("Getting a new spot from a bucket of type " << GetTypeName<BucketType2>().c_str());

	//	auto element = bucketList.find(itemId);

	//	if(element != bucketList.end()) {
	//		//bucket exists
	//		//	Debug("Bucket exists");

	//	}
	//	else {
	//		//create bucket
	//		//Debug("Bucket does not exist");

	//		CreatePool<BucketType2>();
	//	}

	//	void* raw_bucket = bucketList[itemId];
	//	Bucket<BucketType2>* ourBucket = (Bucket<BucketType2>*)raw_bucket;

	//	BucketType2* free_spot = ourBucket->CreateFreeSpot();
	//	//	Debug("Made a free spot in bucket : " << free_spot);

	//	return free_spot;
	//}

	/*template<typename T>
	Bucket<T>* GetBucket() {
		std::string key = GetTypeName<T>();
		void* b = bucketList[key];
		return (Bucket<T>*)b;
	}*/

	const size_t BucketCount() const { return poolListCount; }
};
