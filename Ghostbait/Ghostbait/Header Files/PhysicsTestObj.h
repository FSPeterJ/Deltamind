#pragma once
#include "GameObject.h"
#include "Controlable.h"
#include "MaterialManager.h"

class ParticleManager;

class PhysicsTestObj : public GameObject, public Controlable {
public:
	bool isControllable = false;
	bool isRayCasting = false;

	PhysicsTestObj();
	~PhysicsTestObj();
	void Update() override;
	void OnCollision(GameObject * _other);
	void Awake(GameObject* _other);
	void AnExtremelyBadTemporaryFunction(::ParticleManager* partman, MaterialManager* matman);
};
