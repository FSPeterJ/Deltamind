#pragma once
#include "GameObject.h"

class ForceField : public GameObject {
public:
	bool first = true;
	ForceField();
	void Awake(Object* obj) override;
	void Destroy() override;
	void Update() override;
};