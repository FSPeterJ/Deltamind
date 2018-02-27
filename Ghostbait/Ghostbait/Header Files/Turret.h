#pragma once
#include "GameObject.h"
#include "Animator.h"

class Turret: public GameObject {
	GameObject* target;
	unsigned projectiePID =0;
	float targetDistance =0;
	float firerate =0;
	float damage = 10;
	float timeSinceLastShot = 0;
	DirectX::XMFLOAT4X4* turretPitch;
	unsigned eventDestroy;

public:
	Turret();
	void Enable();
	void Disable();
	void Awake(Object* obj);
	void Update();
	float CalculateDistance(GameObject* obj);
	~Turret();
	void OnTrigger(GameObject* object);
	bool CanShoot(float fireRate);
	void Shoot();
	void GivePID(unsigned pid, const char* tag);
};
