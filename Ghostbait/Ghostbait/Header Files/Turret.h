#pragma once
#include "GameObject.h"

class Turret: public GameObject {
	GameObject* target;
	float targetDistance;
public:
	Turret();
	~Turret();
	void OnCollision(GameObject* object);
};
