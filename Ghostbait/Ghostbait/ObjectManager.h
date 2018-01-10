#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"
#include "ObjectFactory.h"

class ObjectManager: public ManagerInterface {
private:
	struct Pool {
		std::vector<Object*> inactiveList;
		std::vector<Object*> activeList;
	};

	std::unordered_map<int, Object*> masterActive;

	std::vector<Pool> objectPool;

	
public:
	ObjectManager();
	~ObjectManager();

	void CreatePool(int size, Object* poolType) {}


};
