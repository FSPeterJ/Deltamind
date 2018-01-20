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
			{
				DirectX::XMMATRIX pose;
				if (event.trackedDeviceIndex == vrMan->leftController.index) {
					DirectX::XMFLOAT4 temp(vrMan->leftController.pose._41, vrMan->leftController.pose._42, vrMan->leftController.pose._43, vrMan->leftController.pose._44);
					Object* obj;
					MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, temp, &obj));
					DirectX::XMMATRIX scaled = DirectX::XMLoadFloat4x4(&obj->position);

					XMStoreFloat4x4(&obj->position, DirectX::XMMatrixScaling(0.30f, 0.30f, 0.30f) *scaled);
					input = none;
					amount = 0.0f;
				}
				else {
					input = teleport;
					amount = 1;
					DirectX::XMStoreFloat4x4(&vrMan->world, DirectX::XMLoadFloat4x4(&vrMan->world) * DirectX::XMMatrixTranslation(vrMan->hmdPose._31, vrMan->hmdPose._32, vrMan->hmdPose._33));
				}
				break;
			}
			break;
			}
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
	MapKey(forward, 'A');
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	Control input = none;
	float amount = 0;
	
	while (inputQueue.size() > 0) {
		int j = 0;
		for (auto value : keyBind) {
			if (value.second == inputQueue.front()) {
				input = value.first;
				amount = 1;
			}
		}
		inputQueue.pop();
	}

	InputPackage message(input, amount);
	return message;
}
bool InputManager::KeyboardInput::MapKey(Control control, int key) {
	keyBind[control] = key;
	return true;
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
std::queue<uint64_t> InputManager::inputQueue;

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
