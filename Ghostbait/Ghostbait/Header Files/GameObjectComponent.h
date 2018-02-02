#pragma once
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"
#include "Console.h"          // for Console, Console::WriteLine, Console::WriteLiner
#include "ComponentBase.h"

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
