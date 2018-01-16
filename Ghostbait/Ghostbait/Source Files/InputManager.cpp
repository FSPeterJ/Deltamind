#include "InputManager.h"

#include <unordered_map>
#include <assert.h>
#include "Messagebox.h"

//VR
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
bool InputManager::VRInput::MapKey(Control control, int key) {
	if (keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	}
	else { return false; }
}
InputPackage InputManager::VRInput::CheckForInput() {

	Control input = none;
	float amount = 0;

	vr::VREvent_t event;
	while (vrMan->pVRHMD->PollNextEvent(&event, sizeof(event)))
	{
		switch (event.eventType){
		case vr::VREvent_ButtonTouch:
		{
			if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {

			}
			break;
		}
		case vr::VREvent_ButtonUntouch:
		{
			if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {

			}
			break;
		}
		case vr::VREvent_ButtonPress:
		{
			switch (event.data.controller.button) {
			case vr::k_EButton_ApplicationMenu:
				break;
			case vr::k_EButton_Grip:
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				break;
			case vr::k_EButton_SteamVR_Trigger:
				DirectX::XMMATRIX pose;
				if (event.trackedDeviceIndex == vrMan->leftController.index) {
					pose = vrMan->leftController.pose;
				}
				else {
					pose = vrMan->rightController.pose;
				}

				DirectX::XMFLOAT4 temp;
				DirectX::XMStoreFloat4(&temp, pose.r[3]);
				Object* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, temp, &obj));
				obj->position = DirectX::XMMatrixScaling(0.15f, 0.15f, 0.15f) * obj->position;


				input = teleport;
				amount = 0.5f;
				
				break;
			}
			break;
		}
		case vr::VREvent_ButtonUnpress:
		{
			switch (event.data.controller.button) {
			case vr::k_EButton_ApplicationMenu:
				break;
			case vr::k_EButton_Grip:
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				break;
			case vr::k_EButton_SteamVR_Trigger:
				break;
			}
			break;
		}
		}
	}
	
	//vr::VRControllerState_t state;
	//vrMan->pVRHMD->GetControllerState(vrMan->controller1Index, &state, sizeof(state));
	//Console::Write(state.rAxis[0].x);
	//Console::Write(", ");
	//Console::WriteLine(state.rAxis[0].y);


	
	InputPackage message(input, amount);

	return message;
}

//Keyboard
InputManager::KeyboardInput::KeyboardInput() {
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	InputPackage message(none, 1);
	return message;
}
bool InputManager::KeyboardInput::MapKey(Control control, int key) {
	if (keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	}
	else { return false; }
}

//Controller
InputManager::ControllerInput::ControllerInput() {

}
InputPackage InputManager::ControllerInput::CheckForInput() {
	InputPackage message(none, 1);
	return message;
}
bool InputManager::ControllerInput::MapKey(Control control, int key) {
	if(keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	} else { return false; };
}

//Input Manager
InputPackage InputManager::HandleInput() {
	InputPackage input = bridge->CheckForInput();
	
	MessageEvents::SendMessage(EVENT_Input, InputMessage(input.control, input.amount));

	return input;
}
void InputManager::SetInputType(InputType type) {
	inputType = type;
	switch (type) {
	case InputType::VR:
		this->bridge = new VRInput(vrMan);
		break;
	case InputType::KEYBOARD:
		bridge = new KeyboardInput();
		break;
	case InputType::CONTROLLER:
		bridge = new ControllerInput();
		break;
	default:
		Messagebox::ShowError("Input Type Error", "No InputType is defined!");
	}
}
