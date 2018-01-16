#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "MessageEvents.h"
#include "ComponentBase.h"
#include "TypeMapping.h"

#define ALIGN(w) __declspec(align(w))
#define ALIGNMENT 16

ALIGN(ALIGNMENT) class Object
{

protected:

private:

public:

	//NOTE: Should these be moved to private with getter/setter?
	DirectX::XMMATRIX position = DirectX::XMMatrixIdentity();
	TypeMap<ComponentBase> Components;
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
	void SetComponent(ComponentType * _component)
	{
		Components.AddComponent<ComponentType>(_component);
	};

	static int GetTypeId() {	return 0;};
};