#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"


class ObjectManager: public ManagerInterface {
private:
	class Pool {
	private:
		std::vector< Object*> activeList;
		std::vector< Object*> inactiveList; //Linked list with head/tail ptrs
		std::vector<Object> objects;

		void RemoveObjectFromActive(const Object* o) {
			auto it = std::find(activeList.begin(), activeList.end(), o);

			if(it != activeList.end()) {
				std::swap(*it, activeList.back());
				activeList.pop_back();
			}
		}
		Object* Add(const Object* o) {
			objects.push_back(*o);
			activeList.push_back(&objects[objects.size() - 1]);
			return &objects.back();
		}
	public:
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
				return Add(o);
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
