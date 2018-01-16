#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"
#include <array>
#define MAX_POOL_SIZE 16384

class ObjectManager: public ManagerInterface {
private:
	class Pool {
	private:
		std::vector< Object*> activeList;
		std::vector< Object*> inactiveList; //Linked list with head/tail ptrs
		std::array<Object, MAX_POOL_SIZE> objects;

		void RemoveObjectFromActive(const Object* o) {
			auto it = std::find(activeList.begin(), activeList.end(), o);

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
		/// <summary>
		/// Activates the specified object.
		/// </summary>
		/// <param name="o">The object to activate.</param>
		/// <returns>A pointer to the activated or added Object.</returns>
		Object* Activate(const Object* o) {
			if(inactiveList.size()) {
				activeList.push_back(inactiveList[0]);
				inactiveList.erase(inactiveList.begin());
				return activeList.back();
			}
			else {
				throw std::exception("Error: Attempted to activate more objects than the max pool size!");
				return nullptr;
			}
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

	static std::vector<Pool> objectPool;
	static std::unordered_map<const Object*, Pool*> poolScope;

	static void Instantiate(EventMessageBase* e);
	static void Destroy(EventMessageBase* e);

	ObjectManager();
public:
	~ObjectManager();
	static void Initialize();
	static void CreatePool(int _size, Object* poolType) {}
};
