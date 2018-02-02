#pragma once
#include "Item.h"

class Gun: public Item {
public:
	enum FireType {
		AUTO,
		SEMI,
	};
private:
	unsigned projectiePrefabID = 4;
	float dt = 0;
	float timeSinceLastShot = 0;

	//Main Stats

	GameObject* bulletPrefab;
	FireType type = AUTO;
	float fireRate = 2; //shotsPerSecond
	float damage = 1;

	//Energy Stats
	float energyBulletCost = 1; //Energy output per bullet
	float energyWaitCooldown = 5; //How long you go(seconds) without shooting to begin cooling down
	float energyOverheatDelay = 7; //How long you need to wait (seconds) after reaching/exceeding energy limit to begin cooldown
	float energyLimit = 20; //Max amount of energy before overheat

	float currentEnergy = 0;
	float energyOverheatDelayTimeLeft = 0;

	bool AddEnergy(float energy);

public:
	Gun() { state = GUN; SetTag("Gun"); };
	Gun(FireType _type, float _fireRate, float _damage) : type(_type), fireRate(_fireRate), damage(_damage) { state = GUN; SetTag("Gun"); };
	void SetStats(FireType _type, float _fireRate, float _damage) { type = _type; fireRate = _fireRate; damage = _damage; };
	bool Shoot();
	void Update() override;
};
