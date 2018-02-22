#pragma once
#include "GameObject.h"

class Turret: public GameObject {
	GameObject* target;
	float targetDistance =0;
public:
	Turret();
	void Update();
	float CalculateDistance(GameObject* obj);
	~Turret();
	void OnTrigger(GameObject* object);
};
