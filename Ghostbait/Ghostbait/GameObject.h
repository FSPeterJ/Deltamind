#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"

class GameObject: public Object {
public:

	GameObject() {
		EngineStructure::Awake.add([this]() {this->Awake(); });
	}

	virtual void Awake() {}
};


class SomeCoolObject: public GameObject {

	void Awake() {
		WriteLine("I am a cool object being awakened!");
	}

public:
	SomeCoolObject() : GameObject() { }
};
