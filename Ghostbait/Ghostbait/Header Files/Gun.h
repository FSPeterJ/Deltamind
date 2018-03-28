#pragma once
#include "Item.h"
#include "ProgressBar.h"
#include "Light.h"


class Gun: public Item {
public:
	class Overheat {
		float currentEnergy = 0;
		float energyOverheatDelayTimeLeft = 0;
		float timeSinceLastShot = 0;
	protected:
		friend Gun; // possibly rethink this
		unsigned overheatBarPID = 0;
	public:
		Gun* parent;
		ProgressBar* bar;
		float energyBulletCost = 1; //Energy output per bullet
		float energyWaitCooldown = 4; //How long you go(seconds) without shooting to begin cooling down
		float energyOverheatDelay = 6; //How long you need to wait (seconds) after reaching/exceeding energy limit to begin cooldown
		float energyLimit = 25; //Max amount of energy before overheat

		bool AddEnergy(float energy);
		bool CanShoot(float fireRate);
		inline void ResetTimeSinceLastShot() { timeSinceLastShot = 0; };
		void Update(bool active = true);
		void CreateBar(Gun* _parent = nullptr);
	};
	enum FireType {
		AUTO,
		SEMI,
	};
	Light flash;

protected:
	unsigned projectiePID = 0;
private:
	//Main Stats
	FireType type = AUTO;
	float fireRate = 2; //shotsPerSecond
	float damage = 1;

public:
	Gun();
	void Awake(Object * obj);
	void GivePID(unsigned pid, const char* tag) override;
	void SetStats(FireType _type, float _fireRate, float _damage) { type = _type; fireRate = _fireRate; damage = _damage; };
	bool Shoot(bool addOverheat = true);
	void InactiveUpdate() override;
	void ActiveUpdate() override;

	void Selected() override;
	void DeSelected() override;
	Overheat overheat;
	// This is essentially a copy constructor, but since objects are never truely instantiated post-start...
#ifdef _DEBUG
	void SmokeTest() override;
#endif

	void PassObject(unsigned pid, char* tag) {
		if(strcmp(tag, "projectile")) {
			projectiePID = pid;
		}
		else if(strcmp(tag, "overheat")) {
			overheat.overheatBarPID = pid;
		}
	}
};

