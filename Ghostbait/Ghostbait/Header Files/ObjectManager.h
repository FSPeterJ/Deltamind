#pragma once
#include <unordered_map>
#include "IComponentManager.h"
#include "Pool.h"
#include "Delegate.h"
#include "TypeMapping.h"          // for TypeMap
#include "ObjectFactory.h"

class EventMessageBase;
class GameObject;

#define MAX_ENTITY_COUNT 16384

class ObjectManager: public IManager {
	Delegate<> Delete;
	Delegate<> Update_Delegate;
	MemoryManager* memMan;

	std::unordered_map<std::string, _Pool_Base*> objectpool;

	std::unordered_map<Object*, _Pool_Base*> pointers2Bucket;
	_Pool_Base* poolList;

	unsigned poolListCount;
	size_t poolListNewIndex = 0;

	void Destroy(EventMessageBase* e);

public:
	void Update();

	template<typename PoolType>
	void CreatePool(unsigned size) {
		unsigned typeID = TypeMap::GetObjectTypeID<PoolType>();
		if(typeID < poolListCount) {
			// HATE HATE HATE
			Pool<PoolType>* data = new ((char*) poolList + (sizeof(Pool<size_t>) * typeID)) Pool<PoolType>((unsigned) size);
			Delete += [data]()
			{
				data->~Pool<PoolType>();
			};
			/*Update_Delegate += [data]() {
			std::vector<PoolType*>* lst = data->GetActiveList();
			for(size_t i = 0; i < data->GetActiveCount(); ++i) {
			((PoolType*)lst->operator[](i))->Update();
			}
			};*/
		} else {
			throw std::exception("Attempted to allocate a pool at an index larger than the maximum ObjectPool collection size.");
		}
	}
	ObjectManager(MemoryManager* _memMan);
	~ObjectManager();
	GameObject* Instantiate(unsigned typeID);
	void Initialize(size_t prefabCount);
	void Shutdown() const;

	const size_t BucketCount() const { return poolListCount; }
};
