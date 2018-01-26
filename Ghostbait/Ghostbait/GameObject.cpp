#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "GhostTime.h"

GameObject::GameObject() {

}
void GameObject::Activate() {
	EngineStructure::Awake += [=]() {this->Awake(); };
}

void Gun::Update() {
	dt = (float)GhostTime::SmoothDeltaTime();
	timeSinceLastShot += dt;

	if(energyOverheatDelayTimeLeft > 0) {
		energyOverheatDelayTimeLeft -= dt;
		if(energyOverheatDelayTimeLeft <= 0) {
			energyOverheatDelayTimeLeft = 0;
			currentEnergy = 0;
		}
	}
	else if(timeSinceLastShot >= energyWaitCooldown) {
		currentEnergy = 0;
	}

	//Console::Write(currentEnergy);
	//Console::Write("__");
	//Console::Write(energyLimit);
	//Console::Write(" : ");
	//Console::Write(energyOverheatDelayTimeLeft);
	//Console::Write(" : ");
	//Console::WriteLine(timeSinceLastShot);
}
bool Gun::Shoot() {
	switch(type) {
		case AUTO:
			if(timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
				//Fire
				Object* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(2, { 0, 0, 0 }, &obj));
				obj->position = position;
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
				if(!AddEnergy(energyBulletCost)) {
					energyOverheatDelayTimeLeft = energyOverheatDelay;
				}
				timeSinceLastShot = 0;
			}
			break;
		case SEMI:
			if(timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
				//Fire
				Object* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(2, { 0, 0, 0 }, &obj));
				obj->position = position;
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
				if(!AddEnergy(energyBulletCost)) {
					energyOverheatDelayTimeLeft = energyOverheatDelay;
				}
				timeSinceLastShot = 0;
				return false;
			}
			break;
	}
	return true;
}

void ControllerObject::Update() {
	if(hand == INVALID) return;
	else if(hand == LEFT) LeftUpdate();
	else RightUpdate();
}
void ControllerObject::LeftUpdate() {
#pragma region Switch Controller Item
	if(KeyIsDown(leftItem1)) {
		if(items[0]) currentItem = items[0];
		ResetKey(leftItem1);
	}
	else if(KeyIsDown(leftItem2)) {
		if(items[1]) currentItem = items[1];
		ResetKey(leftItem2);
	}
	else if(KeyIsDown(leftItem3)) {
		if(items[2]) currentItem = items[2];
		ResetKey(leftItem3);
	}
	else if(KeyIsDown(leftItem4)) {
		if(items[3]) currentItem = items[3];
		ResetKey(leftItem4);
	}
#pragma endregion
	if(currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch(currentItem->state) {
			case Item::State::GUN:
				if(KeyIsDown(leftAttack)) {
					if(!((Gun*)currentItem)->Shoot()) ResetKey(leftAttack);
				}
				break;
			case Item::State::CONTROLLER:
				if(KeyIsDown(leftAttack)) {
					WriteLine("Right controller select");
					ResetKey(leftAttack);
				}
				break;
			case Item::State::INVALID:
				if(KeyIsDown(leftAttack)) {
					WriteLine("Right hand pickup");
					ResetKey(leftAttack);
				}
				break;
		}
	}
}
void ControllerObject::RightUpdate() {
#pragma region Switch Controller Item
	if(KeyIsDown(rightItem1)) {
		if(items[0]) currentItem = items[0];
		ResetKey(rightItem1);
	}
	else if(KeyIsDown(rightItem2)) {
		if(items[1]) currentItem = items[1];
		ResetKey(rightItem2);
	}
	else if(KeyIsDown(rightItem3)) {
		if(items[2]) currentItem = items[2];
		ResetKey(rightItem3);
	}
	else if(KeyIsDown(rightItem4)) {
		if(items[3]) currentItem = items[3];
		ResetKey(rightItem4);
	}
#pragma endregion
	if(currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch(currentItem->state) {
			case Item::State::GUN:
				if(KeyIsDown(rightAttack)) {
					if(!((Gun*)currentItem)->Shoot()) ResetKey(rightAttack);
				}
				break;
			case Item::State::CONTROLLER:
				if(KeyIsDown(rightAttack)) {
					WriteLine("Right controller select");
					ResetKey(leftAttack);
				}
				break;
			case Item::State::INVALID:
				if(KeyIsDown(rightAttack)) {
					WriteLine("Right hand pickup");
					ResetKey(rightAttack);
				}
				break;
		}
	}
}

Spawner::Spawner() {
	timeSinceLastSpawn = runSpawnDelay - startSpawnDelay;
}
void Spawner::SpawnObject() {
	Object* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0, 0, 0 }, &obj));
	obj->position = position;
	timeSinceLastSpawn = 0;
}
void Spawner::Update() {
	timeSinceLastSpawn += (float)GhostTime::SmoothDeltaTime();
	if(timeSinceLastSpawn >= runSpawnDelay) {
		SpawnObject();
	}
}