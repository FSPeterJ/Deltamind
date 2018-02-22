#pragma once
#include "Item.h"
#include "ProgressBar.h"

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
		float energyWaitCooldown = 5; //How long you go(seconds) without shooting to begin cooling down
		float energyOverheatDelay = 7; //How long you need to wait (seconds) after reaching/exceeding energy limit to begin cooldown
		float energyLimit = 20; //Max amount of energy before overheat

		Overheat();
		bool AddEnergy(float energy);
		bool CanShoot(float fireRate);
		inline void ResetTimeSinceLastShot() { timeSinceLastShot = 0; };
		void Update(bool active = true);
		void CreateBar();
	};
	enum FireType {
		AUTO,
		SEMI,
	};
protected:
	unsigned projectiePID = 0;
	Overheat overheat;
private:
	//Main Stats
	FireType type = AUTO;
	float fireRate = 2; //shotsPerSecond
	float damage = 1;

public:
	Gun();
	Gun(FireType _type, float _fireRate, float _damage);
	void GivePID(unsigned pid, const char* tag) override;
	void SetStats(FireType _type, float _fireRate, float _damage) { type = _type; fireRate = _fireRate; damage = _damage; };
	bool Shoot();
	void InactiveUpdate() override;
	void ActiveUpdate() override;
	// This is essentially a copy constructor, but since objects are never truely instantiated post-start...
	void CloneData(Object* obj);
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
