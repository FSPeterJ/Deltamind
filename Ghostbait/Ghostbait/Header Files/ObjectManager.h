#pragma once
#include "ManagerInterface.h"
#include "Object.h"
#include "MessageEvents.h"
#include "Pool.h"
#define MAX_ENTITY_COUNT 16384

class ObjectManager: public ManagerInterface 
{
	static Pool<Object> *objectpool;

	static void Destroy(EventMessageBase* e);

	ObjectManager();
public:
	~ObjectManager();
	static Object* Instantiate(const Object* e);
	static void Initialize();
	static void Shutdown();
	static void CreatePool(int _size, Object* poolType) {}
};
