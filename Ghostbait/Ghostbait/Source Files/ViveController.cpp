#include "ViveController.h"
#include "Material.h"

ViveController::ViveController() {
	state = CONTROLLER;
	SetTag("Controller");
}

void ViveController::Awake(Object* obj) {
	Item::Awake(obj);
	ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
}

void ViveController::ActiveUpdate() {

}
void ViveController::InactiveUpdate() {
}

