#pragma once
#include "GameObject.h"

class Powerup : public GameObject {
	float currentTime;
	float lifespan;
	float blinkPercent;
	float blinkTimer;
	float currentBlinkTime;
	bool blinkOn;
public:
	Powerup();
	void Awake(Object* obj) override;
	void Update() override;
	void OnTrigger(GameObject* other) override;
};