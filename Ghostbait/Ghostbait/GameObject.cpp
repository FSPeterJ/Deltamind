#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"
#include "PhysicsManager.h"
#include "GameObject.h"

GameObject::GameObject() {
	
}

void GameObject::Activate() {
	EngineStructure::Awake += [=]() {this->Awake(); };
}

void Gun::Update(float _dt) {
	dt = _dt; 
	timeSinceLastShot += dt;

	if (energyOverheatDelayTimeLeft > 0) {
		energyOverheatDelayTimeLeft -= dt;
		if (energyOverheatDelayTimeLeft <= 0) {
			energyOverheatDelayTimeLeft = 0;
			currentEnergy = 0;
		}
	}
	else if (timeSinceLastShot >= energyWaitCooldown) {
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
	switch (type) {
	case AUTO:
		if (timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
			//Fire
			Object* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0, 0, 0 }, &obj));
			obj->position = position;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
			if (!AddEnergy(energyBulletCost)) {
				energyOverheatDelayTimeLeft = energyOverheatDelay;
			}
			timeSinceLastShot = 0;
		}
		break;
	case SEMI:
		if (timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
			//Fire
			Object* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0, 0, 0 }, &obj));
			obj->position = position;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
			if (!AddEnergy(energyBulletCost)) {
				energyOverheatDelayTimeLeft = energyOverheatDelay;
			}
			timeSinceLastShot = 0;
			return false;
		}
		break;
	}
	return true;
}

void LeftControllerObject::Update() {
	#pragma region Switch Controller Item
		if (KeyIsDown(leftItem1)) {
		state = GUN;
		ResetKey(leftItem1);
	}
		else if (KeyIsDown(leftItem2)) {
		state = CONTROLLER;
		ResetKey(leftItem2);
	}
		else if (KeyIsDown(leftItem3)) {
		state = HAND;
		ResetKey(leftItem3);
	}
		else {

	}
	#pragma endregion
	gun.Update();

	switch (state) {
	case GUN:
		if (KeyIsDown(leftAttack)) {
			if (!gun.Shoot()) ResetKey(leftAttack);
			gun.position = position;
		}
		break;
	case CONTROLLER:
		if (KeyIsDown(leftAttack)) {
			WriteLine("Left controller select");
			ResetKey(leftAttack);
		}
		break;
	case HAND:
		if (KeyIsDown(leftAttack)) {
			WriteLine("Left hand pickup");
			ResetKey(leftAttack);
		}
		break;
	}
}
void RightControllerObject::Update() {
	#pragma region Switch Controller Item
		if (KeyIsDown(rightItem1)) {
		state = GUN;
		ResetKey(rightItem1);
	}
		else if (KeyIsDown(rightItem2)) {
		state = CONTROLLER;
		ResetKey(rightItem2);
	}
		else if (KeyIsDown(rightItem3)) {
		state = HAND;
		ResetKey(rightItem3);
	}
		else {

	}
	#pragma endregion
	gun.Update();

	switch (state) {
	case GUN:
		if (KeyIsDown(rightAttack)) {
			gun.Shoot();
			gun.position = position;
		}
		break;
	case CONTROLLER:
		if (KeyIsDown(rightAttack)) {
			WriteLine("Right controller select");
			ResetKey(rightAttack);
		}
		break;
	case HAND:
		if (KeyIsDown(rightAttack)) {
			WriteLine("Right hand pickup");
			ResetKey(rightAttack);
		}
		break;
	}
}
