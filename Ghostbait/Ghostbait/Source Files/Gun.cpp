#include "Gun.h"
#include "Projectile.h"
#include "PhysicsManager.h"
#include "GhostTime.h"

bool Gun::Shoot() {
	switch (type) {
	case AUTO:
		if (timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
			//Fire
			Projectile* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(projectiePrefabID, { 0, 0, 0 }, (Object**)&obj));
			obj->position = position;
			obj->position._41 += obj->position._31 * 0.2f;
			obj->position._42 += obj->position._32 * 0.2f;
			obj->position._43 += obj->position._33 * 0.2f;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
			obj->Enable();
			if (!AddEnergy(energyBulletCost)) {
				energyOverheatDelayTimeLeft = energyOverheatDelay;
			}
			timeSinceLastShot = 0;
		}
		break;
	case SEMI:
		if (timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft) {
			//Fire
			Projectile* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(projectiePrefabID, { 0, 0, 0 }, (Object**)&obj));
			obj->position = position;
			obj->position._41 += obj->position._31 * 0.2f;
			obj->position._42 += obj->position._32 * 0.2f;
			obj->position._43 += obj->position._33 * 0.2f;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
			obj->Enable();
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
void Gun::Update() {
	//This should be a parameter that makes it way all the way from main, don't query per object per frame
	dt = (float)GhostTime::DeltaTime();
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

bool Gun::AddEnergy(float energy) {
	currentEnergy += energy;
	if (currentEnergy >= energyLimit) {
		currentEnergy = energyLimit;
		return false;
	}
	return true;
}