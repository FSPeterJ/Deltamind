#include "Projectile.h"
#include "GhostTime.h"


Projectile::Projectile() {
	tag = "bullet";
}


Projectile::~Projectile() {
}

void Projectile::Update() {
	//This should be a parameter that makes it way all the way from main or some other collection, don't query per object per frame
	if(isAwake) {
		float dt = (float)GhostTime::SmoothDeltaTime();
		timeSinceShot += dt;

		if(timeSinceShot > maxProjectileTime) {
			Destroy();
			timeSinceShot = 0;
		}
	}
}

void Projectile::OnCollision(GameObject* object )
{
	Destroy();
	timeSinceShot = 0;
}