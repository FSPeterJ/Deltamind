#include "Item.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"

Item::Item() {
	SetTag("Item");
};


void Item::SetPhysicsComponent(bool active) {
	if (active == physicsIsOn) return;
	PhysicsComponent* pc = GetComponent<PhysicsComponent>();
	if (pc) pc->isActive = active;
	physicsIsOn = active;
}

void Item::Selected() {
	Render();
	SetPhysicsComponent(true);
}
void Item::DeSelected() {
	UnRender();
	SetPhysicsComponent(false);
}