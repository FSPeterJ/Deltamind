#pragma once
#include "Object.h"
#include "Controlable.h"
#include "Console.h"
//#include <functional>

class GameObject: public Object {
	//Until delegate unsubscribe is fixed
protected:

	std::string tag = "none";
	bool isAwake = false;

public:
	GameObject();

	void Enable();
	virtual void OnCollision(GameObject* obj);
	void Disable();

	void Destroy();

	virtual void Awake() {};
	virtual void Update() {};

	inline const std::string GetTag() const { return tag; };
	inline void SetTag(std::string _tag) { tag = _tag; };
};

class Item: public GameObject {
public:
	enum State {
		INVALID,
		GUN,
		CONTROLLER,
		HAND,
	};
	State state;
	Item() { SetTag("Item"); };
	void Update() {};
};
class Gun: public Item, public Controlable {
//class Gun: public Item {
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

	bool AddEnergy(float energy) {
		currentEnergy += energy;
		if(currentEnergy >= energyLimit) {
			currentEnergy = energyLimit;
			return false;
		}
		return true;
	}

public:
	Gun() { state = GUN; SetTag("Gun"); };
	Gun(FireType _type, float _fireRate, float _damage): type(_type), fireRate(_fireRate), damage(_damage) { state = GUN; SetTag("Gun"); };
	void SetStats(FireType _type, float _fireRate, float _damage) { type = _type; fireRate = _fireRate; damage = _damage; };
	bool Shoot();
	void Update();
};
class ViveController: public Item {
public:
	ViveController() {
		state = CONTROLLER;
		SetTag("Controller");
	}
	void update() {};
};

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		INVALID,
		LEFT,
		RIGHT,
	};
private:

	ControllerHand hand = LEFT;
	std::vector<Item*> items;
	Item* currentItem = nullptr;
	void LeftUpdate();
	void RightUpdate();
public:

	ControllerObject() {
		items.resize(4);
		hand = INVALID;
	}
	void AddGun(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (Object**)&items[itemSlot]));
		((Gun*)items[itemSlot])->SetStats(_fireType, _fireRate, _damage);
		if(!currentItem) currentItem = items[itemSlot];
		else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));
	};
	void AddController(int itemSlot, int prefabID) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (Object**)&items[itemSlot]));
		if(!currentItem) currentItem = items[itemSlot];
		else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	};
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };
	void Update() override;
};

class Spawner: public GameObject {
	float dt = 0;
	float timeSinceLastSpawn = 0;
	int spawnCount = 0;

	int prefabID = 6;
	int objectsToSpawn = 1;
	float startSpawnDelay = 0;
	float runSpawnDelay = 2;

	void SpawnObject();
public:
	Spawner();
	void Update();
};


class MenuCube : public GameObject {
public:
	void OnCollision(GameObject* other){
		if (other->GetTag() == "Bullet") {
			Destroy();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 10, 0, 0 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { -10, 0, 0 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, 10 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, -10 }));
			GameObject* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(8/*Core*/, { 0, 1.5f, 0 }, (Object**)&obj));
			DirectX::XMStoreFloat4x4(&obj->position, DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
		}
	};
};

class CoreCube : public GameObject {
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other) {
		if (other->GetTag() == "enemy") {
			Console::WriteLine("YOU LOSE!");
			Destroy();
		}
	};
};