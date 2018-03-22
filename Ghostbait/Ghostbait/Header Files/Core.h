#pragma once
#include "GameObject.h"
#include "Health.h"

class Core : public GameObject,  public Health {
	bool enemyTouched = false;

	GameObject* healthCubes[8];
public:
	Core() { SetTag("Core"); };
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
	void HurtEvent() override;
	void HealedEvent() override;
	void DeathEvent() override;
};