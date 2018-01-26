#pragma once
#include "Object.h"
#include "Controlable.h"
#include "Console.h"
//#include <functional>

class GameObject: public Object {

public:
	GameObject();

	void Enable();
	void Disable();

	void Destroy();

	virtual void Awake() {}
	virtual void Update() {}
};

class Gun : public GameObject {
public:
	enum FireType {
		AUTO,
		SEMI,
	};
private:
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

	bool AddEnergy(float energy) {
		currentEnergy += energy;
		if (currentEnergy >= energyLimit) {
			currentEnergy = energyLimit;
			return false;
		}
		return true;
	}

public:
	Gun() {};
	Gun(FireType _type, float _fireRate, float _damage) : type(_type), fireRate(_fireRate), damage(_damage) {};
	bool Shoot();
	void Update();
};

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		INVALID,
		LEFT,
		RIGHT,
	};
private:
	enum State {
		GUN,
		CONTROLLER,
		HAND,
	};
	ControllerHand hand = LEFT;
	State state;
	Gun gun;
	void LeftUpdate();
	void RightUpdate();
public:
	
	ControllerObject()
	{
		TypeMap::RegisterObjectAlias<ControllerObject>("ViveController");
		state = GUN;
		hand = INVALID;
		gun = Gun(Gun::FireType::AUTO, 60, 1);
	}
	inline void SetControllerHand(ControllerHand _hand) {hand = _hand;};
	void Update() override;
};

//class LeftControllerObject: public ControllerObject {
//
//public:
//	LeftControllerObject() { 
//		state = GUN; gun = Gun(Gun::FireType::SEMI, 60, 1); 
//	};
//	void Update() override;
//};

//class RightControllerObject: public ControllerObject {
//public:
//	RightControllerObject() { 
//		TypeMap::RegisterObjectAlias<RightControllerObject>("RightViveController");
//		state = GUN; gun = Gun(Gun::FireType::AUTO, 4, 1); 
//	};
//	void Update() override;
//};

class SomeCoolObject: public GameObject, public Controlable {
public:
	void Awake() {
		Console::WriteLine("Hey im being awakened.");
	}
	void Update() {};
};
class SomeLeakyObject: public GameObject {
	int* testing;
public:
	SomeLeakyObject() {
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("SomeLeakyObject");
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("SomeLeaks");
		TypeMap::RegisterObjectAlias<SomeLeakyObject>("LeakyThing");
		testing = new int(66);
	}

	~SomeLeakyObject() {
		delete testing;
	}

	void Awake() override {
		Console::WriteLine("I am a cool object being awakened!");
	}

	void Update() override {
	}
};
