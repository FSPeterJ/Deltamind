#include "Projectile.h"
#include "GhostTime.h"


Projectile::Projectile() {
}


Projectile::~Projectile() {
}

void Projectile::Update() {
	//This should be a parameter that makes it way all the way from main or some other collection, don't query per object per frame
	float dt = (float)GhostTime::SmoothDeltaTime();

	timeSinceShot += dt;

	if(timeSinceShot > maxProjectileTime) {
		MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(this));
	}
}