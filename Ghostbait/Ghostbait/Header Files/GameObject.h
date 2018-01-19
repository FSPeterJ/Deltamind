#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"

class GameObject: public Object {
public:
	//CloneFunction(GameObject)
	CloneFunction(GameObject)

	GameObject() {
		EngineStructure::Awake.add([=]() {this->Awake(); });
		EngineStructure::Update.add([=]() {this->Update(); });
	}
	virtual ~GameObject() {}

	virtual void Awake() {}
	virtual void Update() {}
};




class SomeCoolObject: public GameObject, public Controlable {
	CloneFunction(SomeCoolObject)


	void Awake() {
		WriteLine("I am a cool object being awakened!");
	}

	void Update() {
		if(KeyIsDown(teleport)) {
			Write("teleport down: ");
			WriteLine(Amount(teleport));
		}

	}
};
