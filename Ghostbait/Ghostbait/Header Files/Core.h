#pragma once
#include "GameObject.h"
#include "Health.h"
#include "Light.h"

class Core : public GameObject,  public Health {
	Light light;
	float panicTimer = -1;
	float panicDuration = 4;
	DirectX::XMFLOAT3 panicColor = { 1, 0, 0 };
	DirectX::XMFLOAT3 normalColor = { 1, 1, 1 };

	int cubeCount = 10;
	GameObject* healthCubes[10];
public:
	Core() { SetTag("Core"); };
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
	void HurtEvent() override;
	void HealedEvent() override;
	void DeathEvent() override;
};