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
};
class LeftControllerObject: public ControllerObject {
	int order = 66;
	void Update();
};
class RightControllerObject: public ControllerObject {
	int order = 77;
	void Update();
};

class SomeCoolObject: public GameObject, public Controlable {
	void Awake() {
		WriteLine("Hey im being awakened.");
	}
	void Update() {};
};
class SomeLeakyObject: public GameObject {
	int* testing;
public:
	SomeLeakyObject() {
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
