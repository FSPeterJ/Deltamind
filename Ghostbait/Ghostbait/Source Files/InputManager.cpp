#include "InputManager.h"

InputType InputManager::inputType = VR;
InputManager::InputBridge InputManager::bridge = VRInput();

//Input Constructors
InputManager::VRInput::VRInput() {
	MapKey(NONE, 0);
	MapKey(FORWARD, 1);
	MapKey(BACKWARD, 2);
	MapKey(LEFT, 3);
	MapKey(RIGHT, 4);
	MapKey(TELEPORT, 5);
	MapKey(ATTACK, 6);
	MapKey(MENU, 7);
}
InputManager::KeyboardInput::KeyboardInput() {

}
InputManager::ControllerInput::ControllerInput() {

}

//Input readers
InputPackage InputManager::VRInput::CheckForInput() {
	InputPackage message;
	message.control = Control::NONE;
	message.amount = 1;
	return message;
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	InputPackage message;
	message.control = Control::NONE;
	message.amount = 1;
	return message;
}
InputPackage InputManager::ControllerInput::CheckForInput() {
	InputPackage message;
	message.control = Control::NONE;
	message.amount = 1;
	return message;
}

//Input remapping
bool InputManager::VRInput::MapKey(Control control, int key) {
	if (keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	}
	else { return false; }
}
bool InputManager::KeyboardInput::MapKey(Control control, int key) {
	if (keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	}
	else { return false; }
}
bool InputManager::ControllerInput::MapKey(Control control, int key) {
	if(keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	} else { return false; };
}

InputPackage InputManager::HandleInput() {
	InputPackage input = bridge.CheckForInput();
	
	MessageEvents::SendMessage(EVENT_Input, InputMessage(input.control, input.amount));

	return input;
}

void InputManager::SetInputType(InputType type) {
	inputType = type;
	switch (type) {
	case InputType::VR:
		bridge = VRInput();
		break;
	case InputType::KEYBOARD:
		bridge = KeyboardInput();
		break;
	case InputType::CONTROLLER:
		bridge = ControllerInput();
		break;
	default:
		Messagebox::ShowError(L"Input Type Error", L"No InputType is defined!");
	}
}
