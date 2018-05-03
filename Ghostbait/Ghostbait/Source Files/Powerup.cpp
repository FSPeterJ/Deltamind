#include "Powerup.h"
#include "GhostTime.h"
#include "MessageEvents.h"

Powerup::Powerup() {
	SetTag("Powerup");
}
void Powerup::Awake(Object* obj) {
	currentTime = 0;
	lifespan = 10;
	blinkPercent = 0.4f;
	blinkTimer = 0.045f;
	currentBlinkTime = 0;
	blinkOn = true;
	GameObject::Awake(obj);
	Enable();
	ToggleFlag(UNLIT);
}
void Powerup::Update() {
	GameObject::Update();
	currentTime += (float)GhostTime::DeltaTimeNoScale();
	transform.MoveAlongUp(sinf(currentTime) * 0.1f);
	transform.SetRotationRadians(0, currentTime, 0);
	float life = currentTime / lifespan;
	if (currentTime >= lifespan) {
		Render();
		Destroy();
	}
	else if (life >= (1 - blinkPercent)) {
		currentBlinkTime += (float)GhostTime::DeltaTimeNoScale();
		if (currentBlinkTime >= blinkTimer) {
			currentBlinkTime = 0;
			if (blinkOn) 
				UnRender();
			else 
				Render();
			blinkOn = !blinkOn;
		}
	}
}
void Powerup::OnTrigger(GameObject* other) {
	GameObject::OnCollision(other);
	if (other->GetTag() == "Controller") {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {
			Destroy();
			GhostTime::TurnOnSloMo();
		});
	}
}