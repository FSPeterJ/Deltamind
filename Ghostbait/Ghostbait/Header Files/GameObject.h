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

class Gun : public GameObject {
	enum FireType {
		AUTO,
		BURST,
		SEMI,
	};

	FireType type;
	GameObject* bulletPrefab;
	float damage;
	float overHeat;

	void Update();
};

class ControllerObject: public GameObject, public Controlable {
public:
	ControllerObject()
	{
		TypeMap::RegisterObjectAlias<ControllerObject>("ViveController");
	}
public:
	enum State {
		GUN,
		CONTROLLER,
		HAND,
	};
	State state;
	Gun* gun;
};

class LeftControllerObject: public ControllerObject {

public:
	LeftControllerObject() { state = GUN; };
	void Update();
};

class RightControllerObject: public ControllerObject {
public:
	RightControllerObject() { state = GUN; };
	void Update();
};

class SomeCoolObject: public GameObject, public Controlable {
public:
	void Awake() {
		Console::WriteLine("Hey im being awakened.");
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
		Console::WriteLine("I am a cool object being awakened!");
	}

	void Update() override {
	}
};
