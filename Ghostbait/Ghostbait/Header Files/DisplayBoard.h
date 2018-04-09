#pragma once
#include "GameObject.h"

class DisplayBoard : public GameObject {
	GameObject* lifter = nullptr;
	GameObject* screen = nullptr;
public:
	DisplayBoard() {}
	~DisplayBoard() {}

	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
};