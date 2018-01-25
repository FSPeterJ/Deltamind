#pragma once
#include "Object.h"
#include "Controlable.h"
#include "Console.h"

class GameObject: public Object {
public:
	GameObject();
	void Activate();
	virtual void Awake() {}
	virtual void Update() {}
};

class ControllerObject: public GameObject, public Controlable {
public:
	ControllerObject()
	{
		TypeMap::RegisterObjectAlias<ControllerObject>("ViveController");
	}
};

class LeftControllerObject: public ControllerObject {

public:
	void Update();
};

class RightControllerObject: public ControllerObject {
public:

	void Update();
};

class SomeCoolObject: public GameObject, public Controlable {
public:
	void Awake() {
		WriteLine("Hey im being awakened.");
	}
	void Update() {};
};
class SomeLeakyObject: public GameObject {
	int* testing;
public:
	SomeLeakyObject() {
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("SomeLeakyObject");
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("SomeLeaks");
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("LeakyThing");
		testing = new int(66);
	}

	~SomeLeakyObject() {
		delete testing;
	}

	void Awake() override {
		WriteLine("I am a cool object being awakened!");
	}

	void Update() override {
	}
};
