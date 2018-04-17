#pragma once
#include "GameObject.h"

static int instantiatedCount = 0;
static int destroyedCount = 0;

struct PhysicsComponent;

class Projectile: public GameObject {
public:
	bool isDestroying = false;
	float timeSinceShot = 0;
	float maxProjectileTime = 4.0f;
	float damage = 1;

	PhysicsComponent* pc = nullptr;

	Projectile();
	~Projectile();

	void Awake(Object* obj) override;
	void Update() override;
	void OnCollision(GameObject * object);
	void SetDamage(float _damage);
	void Destroy() override;
};
