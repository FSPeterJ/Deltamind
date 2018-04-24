#pragma once
#include "GameObject.h"

struct PhysicsComponent;

class Item: public GameObject {
protected:
	bool physicsIsOn = true;
	PhysicsComponent* pc = nullptr;
public:
	enum State {
		INVALID,
		GUN,
		CONTROLLER,
		BUILD,
		PDA,
		HAND,
	};
	State state;
	Item();

	void Awake(Object* obj) override;
	virtual void SetPhysicsComponent(bool active = true);
	virtual void InactiveUpdate() {}
	virtual void ActiveUpdate() {}

	virtual void Selected();
	virtual void DeSelected();

	virtual void SetDisplay() {}
};
