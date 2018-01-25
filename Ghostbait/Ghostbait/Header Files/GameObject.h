#pragma once
#include "Object.h"
#include "Controlable.h"
#include "Console.h"

class GameObject: public Object {
public:
	GameObject();
	void Activate();
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
	void Update(float _dt = 0.02f);
};

class ControllerObject: public GameObject, public Controlable {
public:
	ControllerObject()
	{
		TypeMap::RegisterObjectAlias<ControllerObject>("ViveController");
	}
public:
	enum State {
		GUN,
		CONTROLLER,
		HAND,
	};
	State state;
	Gun gun;
};

class LeftControllerObject: public ControllerObject {

public:
	LeftControllerObject() { state = GUN; gun = Gun(Gun::FireType::SEMI, 60, 1); };
	void Update();
};

class RightControllerObject: public ControllerObject {
public:
	RightControllerObject() { state = GUN; gun = Gun(Gun::FireType::AUTO, 4, 1); };
	void Update();
};

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
