#include "Item.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"

Item::Item() {
	SetTag("Item");
};

void Item::Render(bool render) {
	if (render == isRendered) return;

	if (render) {
		MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(this));
		isRendered = true;
	}
	else {
		MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(this));
		isRendered = false;
	}
}

void Item::SetPhysicsComponent(bool active) {
	if (active == physicsIsOn) return;
	PhysicsComponent* pc = GetComponent<PhysicsComponent>();
	if (pc) pc->isActive = active;
	physicsIsOn = active;
}

void Item::Selected() {
	Render(true);
	SetPhysicsComponent(true);
}
void Item::DeSelected() {
	Render(false);
	SetPhysicsComponent(false);
}