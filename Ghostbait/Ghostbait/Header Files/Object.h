#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "MessageEvents.h"
#include <iostream>

using namespace DirectX;

#define ALIGN(w) __declspec(align(w))
#define ALIGNMENT 16

ALIGN(ALIGNMENT) class Object {
protected:
	
private:
	
public:
	XMMATRIX object = XMMatrixIdentity();

	Object() {};
	virtual ~Object() {};

	void* operator new(size_t i) { return _mm_malloc(i, ALIGNMENT); }

	void operator delete(void* p) { _mm_free(p); }

	//void testing() { Console::WriteLine("object"); };
};
