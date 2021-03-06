#include "CoreShield.h"
#include "MessageEvents.h"

CoreShield::CoreShield() {
	SetTag("Wall");
}

void CoreShield::Update() {
	GameObject::Update();
}

void CoreShield::OnCollision(GameObject* other) {
	if (other->GetTag() == "Bullet") {
		MessageEvents::SendQueueMessage(EVENT_Late, [&] { 
			this->Destroy(); 
			MessageEvents::SendMessage(EVENT_Start, EventMessageBase());
		});
	}
	GameObject::OnCollision(other);
}
