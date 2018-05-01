#pragma once
#include "GameObject.h"

class DMLogo : public GameObject {
	float speed;
	float rot;
	GameObject* text;
public:
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
};