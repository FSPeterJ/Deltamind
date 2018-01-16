#pragma once
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"
#include <array>

#define MAX_POOL_SIZE 128

class ObjectManager {
private:
	class PoolCluster
	{

		class Pool {
			std::vector< Object*> activeList;
			std::vector< Object*> inactiveList; //Linked list with head/tail ptrs
			std::array<Object, MAX_POOL_SIZE> objects;

			void RemoveObjectFromActive(const Object* o) {
				const auto it = std::find(activeList.begin(), activeList.end(), o);

				if(it != activeList.end()) {
					std::swap(*it, activeList.back());
					activeList.pop_back();
				}
			}
		public:
			Pool(int size = MAX_POOL_SIZE)
			{
				for(Object &element : objects)
				{
					element = Object();
					inactiveList.push_back(&element);
				}
			}
			bool hasSpace() const
			{
				return activeList.size() < MAX_POOL_SIZE;
			}
			/// <summary>
			/// Activates the specified object.
			/// </summary>
			/// <param name="o">The object to activate.</param>
			/// <returns>A pointer to the activated or added Object.</returns>
			Object* Activate(const Object* o) {
				activeList.push_back(inactiveList[0]);
				inactiveList.erase(inactiveList.begin());
				return activeList.back();
			}
			/// <summary>
			/// Deactivates the specified object.
			/// </summary>
			/// <param name="o">The object to deactivate.</param>
			void Deactivate(Object* o) {
				RemoveObjectFromActive(o);
				inactiveList.push_back(o);
			}
		};
		std::vector<Pool*> objectsubpools;
	public:
		PoolCluster()
		{
			objectsubpools.push_back(new Pool);
		}
		Object* ActivateObject(const Object* o)
		{
			for(Pool* pool : objectsubpools)
			{
				if(pool->hasSpace())
				{
					return pool->Activate(o);
				}
			}
			//This is bad and should happen before a pool runs out of space
			objectsubpools.push_back(new Pool);
			return ActivateObject(o);
		}
		// This should happen on a seperate thread probably.
		void DeactivateObject(Object* o)
		{
			for(Pool* pool : objectsubpools)
			{
				pool->Deactivate(o);
			}

		}
	};
	static std::vector<PoolCluster> objectPool;
	static std::unordered_map<const Object*, PoolCluster*> poolScope;

	static void Instantiate(EventMessageBase* e);
	static void Destroy(EventMessageBase* e);

	ObjectManager();
public:
	~ObjectManager();
	static void Initialize();
	static void CreatePool(int _size, Object* poolType) {}
};
