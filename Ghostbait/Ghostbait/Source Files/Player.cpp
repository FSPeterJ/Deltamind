#include "Player.h"
#include "InputManager.h"
#include "VRManager.h"

Player::Player(InputManager* _im) : im(_im) {
	Enable(false);
}

void Player::Update() {
	if (!im) return;

	switch (im->GetInputType()) {
		case InputType::VR:
			position = VRManager::GetInstance().GetPlayerPosition();
			break;
		case InputType::KEYBOARD:
			break;
	}
}

void Player::LoadControllers(VRControllerTypes type) {
	if (!im) {
		Console::ErrorLine << "Input Manager not set on player!";
		return;
	}
	if (im->GetInputType() != InputType::VR) {
		Console::ErrorLine << "You tried to give the player controllers not in VR!";
		return;
	}

	switch (type) {
		case VRControllerTypes::CONTROLLER_Full:
			//Load in ID's from file
			VRManager::GetInstance().CreateControllers();
			break;
		case VRControllerTypes::CONTROLLER_ModelsOnly:
			break;
	}
}