#include "InputManager.h"

#include <unordered_map>
#include <assert.h>
#include "Messagebox.h"

InputType InputManager::inputType = VR;
InputManager::InputBridge InputManager::bridge = VRInput();

//Input Constructors
InputManager::VRInput::VRInput(VRManager* vrManager) {
	vrMan = vrManager;
	MapKey(none, 0);
	MapKey(forward, 1);
	MapKey(backward, 2);
	MapKey(left, 3);
	MapKey(right, 4);
	MapKey(teleport, 5);
	MapKey(attack, 6);
	MapKey(menu, 7);
}
InputManager::KeyboardInput::KeyboardInput() {
}
InputManager::ControllerInput::ControllerInput() {

}

//Input readers
InputPackage InputManager::VRInput::CheckForInput() {
	for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++)
	{
		vr::VRControllerState_t state;
		
		if (vrMan->pVRHMD->GetControllerState(deviceIndex, &state, sizeof(state)))
		{
			//m_rbShowTrackedDevice[unDevice] = state.ulButtonPressed == 0;
		}
	}
	
	
	InputPackage message(none, 1);

	return message;
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	InputPackage message(none, 1);
	return message;
}
InputPackage InputManager::ControllerInput::CheckForInput() {
	InputPackage message(none, 1);
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
		Messagebox::ShowError("Input Type Error", "No InputType is defined!");
	}
}
