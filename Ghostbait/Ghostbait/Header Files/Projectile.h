#pragma once
#include "GameObject.h"

static int instantiatedCount = 0;
static int destroyedCount = 0;

class Projectile: public GameObject {
public:
	float timeSinceShot = 0;
	float maxProjectileTime = 4.0f;
	float damage = 1;

	Projectile();
	~Projectile();

	void Awake(Object* obj) override;
	void Update() override;
	void OnCollision(GameObject * object);
	void SetDamage(float _damage);
	void Destroy() override;
};
