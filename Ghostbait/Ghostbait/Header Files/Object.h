#pragma once
#include <directxmath.h>
#include "MessageEvents.h"
#include "ComponentBase.h"
#include <unordered_map>
#include "Messagebox.h"
#include <sstream>
#include "StdHeader.h"
#include <new>
#include "Pool.h"

#define MakePrototype(type) Object* type ## _prototype = Object::AddPrototype(#type, new type());


//#define CloneFunction(type) void CloneTo(type* location) const { location = new type(*this); }

//#define CloneFunction(type) void CloneTo(Object* location) const { new(location) type; }

#define CloneFunction(type) void CloneTo(void* location) const { memcpy(location, &type(), sizeof(type)); }



#define ALIGN(w) __declspec(align(w))
#define ALIGNMENT 16

ALIGN(ALIGNMENT) 
class Object
{
private:

protected:


public:
	static std::unordered_map<std::string, Object*> protoTable;

	static std::string GetObjectTypeName(Object* o);

	static Pool objectPool;


	virtual void CloneTo(void* location) const  = 0;

	template< typename T> static T* CreateObject();

	/// <summary>
	/// Do not call this directly. Use MakePrototype macro instead.
	/// </summary>
	/// <param name="type">The type.</param>
	/// <param name="o">The o.</param>
	/// <returns>Object *.</returns>
	static Object* AddPrototype(std::string type, Object* o);








	//NOTE: Should these be moved to private with getter/setter?
	DirectX::XMMATRIX position = DirectX::XMMatrixIdentity();
	ComponentBase* Components[MAX_COMPONENTS];
	//Endnote

	Object() {};
	virtual ~Object() {};

	void* operator new(size_t _i) { return _mm_malloc(_i, ALIGNMENT); }

	void operator delete(void* _p) { _mm_free(_p); }

	ComponentBase* GetComponent(const int _componentID) { return Components[_componentID]; };
	void SetComponent(const int _componentId, ComponentBase* _component) { Components[_componentId] = _component; };
};


template<typename T>
T* Object::CreateObject() {
	Object* prefab = 0;
	std::string name = GetTypeName<T>();
	auto element = protoTable.find(name);

	if(element != protoTable.end()) {
		prefab = element->second;

		T* createdObject = objectPool.GetSpot<T>(name);

		prefab->CloneTo((void*)createdObject);

		return createdObject;
	}

	Messagebox::ShowError("Invalid prototype.", "Type \"" + name + "\" not found.");
	return 0;
}


