#pragma once
#include "GameObject.h"

class Door : public GameObject {
	GameObject* wall = nullptr;
	bool first = true;
public:
	void Awake(Object* obj) override;
	void Update() override;
};