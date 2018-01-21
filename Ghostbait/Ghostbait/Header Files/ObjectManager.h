#pragma once
#include "IComponentManager.h"
#include "Object.h"
#include "MessageEvents.h"
#include "Pool.h"
#define MAX_ENTITY_COUNT 16384

class ObjectManager: public IManager
{
	Delegate<> Delete;
	MemoryManager* memMan;

	std::unordered_map<std::string, _Pool_Base*> objectpool;

	std::unordered_map<Object*, _Pool_Base*> pointers2Bucket;
	_Pool_Base* poolList;

	size_t poolListCount;

	void Destroy(EventMessageBase* e);

	template<typename PoolType>
	void CreateBucket()
	{
		_Pool_Base** address = &poolList[TypeMap::getTypeId<PoolType>()];
		if(address == nullptr)
		{
			Pool<PoolType>* pool = new Pool<PoolType>(bucketSize);
			*address = pool;
			Delete.add([=]() { delete pool; });
		}
	}
public:
	ObjectManager(MemoryManager* _memMan, size_t prefabCount);
	~ObjectManager();
	Object* Instantiate(int typeID);
	void Initialize();
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

	//		CreateBucket<BucketType2>();
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
