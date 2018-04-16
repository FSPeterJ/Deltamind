#pragma once
#include "GameObject.h"
#include "Animator.h"
#include "Health.h"

class Animator;

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

	Animator* anim = nullptr;

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
	virtual void Shoot();
	void GivePID(unsigned pid, const char* tag);
	inline const unsigned GetBuildCost() const { return buildCost; }
};

class Turret_Long : public Turret {
public:
	void Awake(Object* obj) override;
	void Shoot() override;
};
class Turret_Medium : public Turret {
public:
	void Awake(Object* obj) override;
	void Shoot() override;
};
class Turret_Short : public Turret {
	struct Offset {
		float dx = 0, dy = 0, dz = 0;
		Offset() {}
		Offset(float x, float y, float z) : dx(x), dy(y), dz(z) {}
	};
	Offset offset[4] = {Offset(0.25f, 0.4f, 0.45f), Offset(-0.2f, 0.4f, 0.45f), Offset(0.25f, 0.1f, 0.45f), Offset(-0.2f, 0.1f, 0.45f) };
	int currentIndex = 0;
public:
	void Awake(Object* obj) override;
	void Shoot() override;
};
