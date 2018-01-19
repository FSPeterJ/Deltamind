#pragma once
#include <directxmath.h>
#include "MessageEvents.h"
#include "ComponentBase.h"
#include "TypeMapping.h"
#include "TypeContainer.h"

#define ALIGN(w) __declspec(align(w))
#define ALIGNMENT 16

ALIGN(ALIGNMENT) class Object
{

protected:

private:

public:

	DirectX::XMMATRIX position = DirectX::XMMatrixIdentity();
	TypeContainer<ComponentBase*> Components;
	//Endnote

	Object() {};
	virtual ~Object() {};

	void* operator new(size_t _i) { return _mm_malloc(_i, ALIGNMENT); }

	void operator delete(void* _p) { _mm_free(_p); }

	template<typename ComponentType>
	ComponentType* GetComponent()
	{
		return Components.GetComponent<ComponentType>();
	};

	template<typename ComponentType>
	ComponentType* GetComponent() const
	{
		return Components.GetComponent<ComponentType>();
	};
	
	//ComponentBase* GetComponent(const std::string &componentname)
	//{
	//	return Components.GetComponent<>(componentname);
	//};

	template<typename ComponentType>
	int SetComponent(ComponentType * _component)
	{
		return Components.AddComponent<ComponentType>(_component);
	};


	int SetComponent(ComponentBase* _component, const int _id)
	{
		return Components.AddComponent(_component, _id);
	};




	static int GetTypeId() {	return 0;};
};