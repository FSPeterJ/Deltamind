#pragma once
#include "GameObject.h"

class Projectile : public GameObject {
public:
	Projectile();
	~Projectile();
	void Update();
	float timeSinceShot = 0;
	float maxProjectileTime = 4.0f;
};

