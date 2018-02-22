#include "ViveController.h"

ViveController::ViveController() {
	state = CONTROLLER;
	SetTag("Controller");
}

void ViveController::ActiveUpdate() {
}
void ViveController::InactiveUpdate() {
}

void ViveController::CloneData(Object* obj) {
}

