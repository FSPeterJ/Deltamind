#pragma once
#include "GameObject.h"

class Projectile: public GameObject {
public:
	float timeSinceShot = 0;
	float maxProjectileTime = 4.0f;
	float damage = 1;

	Projectile();
	~Projectile();

	void Update();
	void OnCollision(GameObject * object);
	void SetDamage(float _damage);
};
