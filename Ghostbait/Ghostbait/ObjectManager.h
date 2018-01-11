#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"
#include "ObjectFactory.h"

class ObjectManager: public ManagerInterface {
private:
	class Pool {
	private:
		std::vector<Object*> activeList;
		std::vector<Object*> inactiveList; //Linked list with head/tail ptrs
		std::vector<Object> objects;

		void RemoveObjectFromActive(Object* o) {
			auto it = std::find(activeList.begin(), activeList.end(), o);

			if(it != activeList.end()) {
				std::swap(*it, activeList.back());
				activeList.pop_back();
			}
		}
	public:
		void Add(Object* o) {
			objects.push_back(*o);
			activeList.push_back(&objects[objects.size() - 1]);
		}
		bool Activate(Object* o) {
			if(inactiveList.size()) {
				activeList.push_back(inactiveList[0]);
				inactiveList.erase(inactiveList.begin());
				return false;
			} else {
				Add(o);
				return true;
			}
		}
		void Deactivate(Object* o) {
			RemoveObjectFromActive(o);
			inactiveList.push_back(o);
		}
	};

	static std::vector<Pool> objectPool;
	static std::unordered_map<Object*, Pool*> poolScope;

	static void Instantiate(EventMessageBase e);
	static void Destroy(EventMessageBase e);

public:
	ObjectManager();
	~ObjectManager();

	void CreatePool(int size, Object* poolType) {}
};
