#pragma once
#include "GameObject.h"
#include "Animator.h"
#include "Health.h"

class Turret: public GameObject, public Health {
protected:
	GameObject* target;
	unsigned projectiePID =0;
	unsigned eventDestroy;
	float targetDistance =0;
	float firerate = 2;
	float damage = 10;
	float timeSinceLastShot = 0;
	unsigned buildCost = 500;
	DirectX::XMFLOAT4X4* turretPitch;
	DirectX::XMFLOAT4X4* launcherorigin;
	DirectX::XMFLOAT4X4* turretYaw;

	DirectX::XMFLOAT4X4 prevPitch;
	DirectX::XMFLOAT4X4 prevYaw;
	bool hasTargeted = false;

public:

	Turret();
	void Enable() override;
	void Disable();
	void Awake(Object* obj);
	void Update();
	void Destroy() override;
	float CalculateDistance(GameObject* obj);
	~Turret();
	void OnTrigger(GameObject* object);
	bool CanShoot(float fireRate);
	void Shoot();
	void GivePID(unsigned pid, const char* tag);
	inline const unsigned GetBuildCost() const { return buildCost; }
};

class Turret_Long : public Turret {
public:
	void Awake(Object* obj) override;
};
class Turret_Medium : public Turret {
public:
	void Awake(Object* obj) override;
};
class Turret_Short : public Turret {
public:
	void Awake(Object* obj) override;
};
