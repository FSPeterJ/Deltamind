#pragma once
#include "GameObject.h"
#include "Controlable.h"

class PhysicsTestObj : public GameObject, public Controlable {
public:
	bool isControllable = false;
	bool isRayCasting = false;

	PhysicsTestObj();
	~PhysicsTestObj();
	void Update() override;
	void OnCollision(GameObject * _other);
};
