#pragma once
#include "GameObject.h"

class Projectile : public GameObject {
public:
	Projectile();
	~Projectile();
	void Update();
	float timeSinceShot;
	float maxProjectileTime = 3.0;
};

