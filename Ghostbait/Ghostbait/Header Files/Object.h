#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "MessageEvents.h"
#include "ComponentBase.h"
#include <iostream>
#include "ObjectFactoryInterface.h"

enum ComponentsListing
{
	MESH,
	ANIMATION,
	MATERIAL,
	MAX_COMPONENTS
};

#define ALIGN(w) __declspec(align(w))
#define ALIGNMENT 16

ALIGN(ALIGNMENT) class Object : public ObjectFactoryInterface
{

protected:

private:

public:
	DirectX::XMMATRIX object = DirectX::XMMatrixIdentity();
	ComponentBase* Components[MAX_COMPONENTS];
	Object() {};
	virtual ~Object() {};

	void* operator new(size_t _i) { return _mm_malloc(_i, ALIGNMENT); }

	void operator delete(void* _p) { _mm_free(_p); }

	ComponentBase* GetComponent(const int _componentID) { return Components[_componentID]; };
	void SetComponent(const int _componentId, ComponentBase* _component) { Components[_componentId] = _component; };
};