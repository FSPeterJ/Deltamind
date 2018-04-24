#include "Item.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"

Item::Item() {
	SetTag("Item");
};

void Item::Awake(Object* obj) {
	GameObject::Awake(obj);
	pc = GetComponent<PhysicsComponent>();
}

void Item::SetPhysicsComponent(bool active) {
	if (active == physicsIsOn) return;
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