#pragma once
#include "GameObject.h"
#include "Controlable.h"

class PhysicsTestObj : public GameObject, public Controlable {
public:
	bool isControllable = false;

	PhysicsTestObj();
	~PhysicsTestObj();
	void Update();
	void OnCollision(GameObject * _other);
};
