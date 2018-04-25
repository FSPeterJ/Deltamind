#pragma once
#include "GameObject.h"
#include "Health.h"
#include "Light.h"

struct Material;

class Core : public GameObject,  public Health, public GridBasedObject {
	GameObject* coreRing = nullptr;
	Material* coreRingMat = nullptr;
	//DirectX::XMFLOAT4 foreground = { 1, 36.0f / 255.0f, 14.0f / 255.0f, 226.0f / 255.0f };
	DirectX::XMFLOAT4 foreground = { 1, 1, 1, 1 };
	DirectX::XMFLOAT4 background = { 1, 132.0f / 255.0f, 10.0f / 255.0f, 105.0f / 255.0f };

	Light light;
	Light spots[6];
	float panicTimer;
	float panicDuration;
public:
	Core() { SetTag("Core"); };
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
	void HurtEvent() override;
	void HealedEvent() override;
	void DeathEvent() override;
};