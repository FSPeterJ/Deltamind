#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"

class GameObjectComponent: public ComponentBase {
public:

	GameObjectComponent() {
		EngineStructure::Awake += [=]() {this->Awake(); };
		EngineStructure::Update += [=]() {this->Update(); };
	}

	virtual void Awake() {}
	virtual void Update() {}
};

class SomeCoolComponent: public GameObjectComponent, public Controlable {
	void Awake() {
		Console::WriteLine << "I am a cool object being awakened!";
	}

	void Update() {
		if(KeyIsDown(teleport)) {
			Console::WriteLine << "teleport down: " << Amount(teleport);
		}
	}
};
