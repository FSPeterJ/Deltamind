#include "Gun.h"
#include "Projectile.h"
#include "PhysicsManager.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Wwise_IDs.h"


Gun::Overheat::Overheat() {

}
void Gun::Overheat::CreateBar() {
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ProgressBar>(15, { 0.0f, 0.0f, 0.0f }, &bar)); // stop using magic number prefab ID
}
bool Gun::Overheat::CanShoot(float fireRate) {
	return timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft;
}
bool Gun::Overheat::AddEnergy(float energy) {
	currentEnergy += energy;
	if(currentEnergy >= energyLimit) {
		currentEnergy = energyLimit;
		energyOverheatDelayTimeLeft = energyOverheatDelay;
		return false;
	}
	return true;
}
void Gun::Overheat::Update(bool active) {


	//Update Overheat Stats
	float dt = (float)GhostTime::DeltaTime();
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

	// This seems weird but I have no better ideas at the moment
	if(active) {
		//Update Bar itself
		bar->position = parent->position;
		bar->position._41 -= bar->position._11 * 0.04f;
		bar->position._42 -= bar->position._12 * 0.04f;
		bar->position._43 -= bar->position._13 * 0.04f;



		bar->position._41 -= bar->position._31 * 0.1f;
		bar->position._42 -= bar->position._32 * 0.1f;
		bar->position._43 -= bar->position._33 * 0.1f;

		bar->SetBarPercentage(currentEnergy / energyLimit);
	}
	else bar->SetBarPercentage(0);

	//Console Write
	//Console::WriteLine << currentEnergy << "__" << energyLimit << " : " << energyOverheatDelayTimeLeft << " : " << timeSinceLastShot;
}

Gun::Gun() {
	state = GUN;
	SetTag("Gun");
	overheat.parent = this;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
}
Gun::Gun(FireType _type, float _fireRate, float _damage): type(_type), fireRate(_fireRate), damage(_damage) {
	state = GUN;
	SetTag("Gun");
	overheat.parent = this;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
}

void Gun::GivePID(unsigned pid, const char* tag) {
	if(strcmp(tag, "projectile") == 0) {
		projectiePrefabID = pid;
	}
}

bool Gun::Shoot() {
	switch(type) {
		case AUTO:
			if(overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePrefabID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				obj->position = position;
				obj->position._41 += obj->position._31 * 0.2f;
				obj->position._42 += obj->position._32 * 0.2f;
				obj->position._43 += obj->position._33 * 0.2f;
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 10.0f, position._32 * 10.0f, position._33 * 10.0f);
				obj->SetDamage(damage);
				obj->Enable();
				overheat.AddEnergy(overheat.energyBulletCost);
				overheat.ResetTimeSinceLastShot();
			}
			break;
		case SEMI:
			if(overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePrefabID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				obj->position = position;
				obj->position._41 += obj->position._31 * 0.2f;
				obj->position._42 += obj->position._32 * 0.2f;
				obj->position._43 += obj->position._33 * 0.2f;
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 10.0f, position._32 * 10.0f, position._33 * 10.0f);
				obj->SetDamage(damage);
				obj->Enable();
				overheat.AddEnergy(overheat.energyBulletCost);
				overheat.ResetTimeSinceLastShot();
				return false;
			}
			break;
	}
	return true;
}
void Gun::InactiveUpdate() {
	overheat.Update(false);
}
void Gun::ActiveUpdate() {
	overheat.Update(true);
}

void Gun::CloneData(Object* obj) {
	projectiePrefabID = ((Gun*)obj)->projectiePrefabID;
}


