#pragma once
#include "GameObject.h"

class Projectile: public GameObject {
public:
	Projectile();
	~Projectile();
	void Update();
	void OnCollision(GameObject * object);
	float timeSinceShot = 0;
	float maxProjectileTime = 4.0f;
};
