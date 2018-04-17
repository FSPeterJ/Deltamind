#pragma once
#include "GameObject.h"

class SmokeWall :public GameObject {
public:
	SmokeWall();
	void Awake(Object* obj) override;
	void Destroy() override;
};