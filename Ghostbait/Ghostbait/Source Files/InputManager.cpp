#include "InputManager.h"

#include <unordered_map>
#include <assert.h>
#include "Messagebox.h"
#include "PhysicsComponent.h"

//VR
InputManager::VRInput::VRInput(VRManager* vrManager) {
	vrMan = vrManager;
	MapKey(none, 0);
	MapKey(forward, 1);
	MapKey(backward, 2);
	MapKey(left, 3);
	MapKey(right, 4);
	MapKey(teleport, 5);
	MapKey(leftAttack, 6);
	MapKey(rightAttack, 7);
	MapKey(menu, 8);
}
bool InputManager::VRInput::MapKey(Control control, int key) {
	if(keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	} else { return false; }
}
InputPackage InputManager::VRInput::CheckForInput() {
	Control input = none;
	float amount = 0;

	vr::VREvent_t event;
	while(vrMan->pVRHMD->PollNextEvent(&event, sizeof(event))) {
		switch(event.eventType) {
		case vr::VREvent_ButtonTouch:
		{
			if(event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
			}
			break;
		}
		case vr::VREvent_ButtonUntouch:
		{
			if(event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
			}
			break;
		}
		case vr::VREvent_ButtonPress:
		{
			switch(event.data.controller.button) {
			case vr::k_EButton_ApplicationMenu:
				if(event.trackedDeviceIndex == vrMan->leftController.index) {
				} else {
					input = teleport;
					amount = 1;
					DirectX::XMStoreFloat4x4(&vrMan->world, DirectX::XMLoadFloat4x4(&vrMan->world) * DirectX::XMMatrixTranslation(vrMan->hmdPose._31, vrMan->hmdPose._32, vrMan->hmdPose._33));
				}
				break;
			case vr::k_EButton_Grip:
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				break;
			case vr::k_EButton_SteamVR_Trigger:
				if(event.trackedDeviceIndex == VRManager::leftController.index) {
					//DirectX::XMFLOAT4 temp(vrMan->leftController.pose._41, vrMan->leftController.pose._42, vrMan->leftController.pose._43, vrMan->leftController.pose._44);
					//Object* obj;
					//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0, 0, 0, 0 }, &obj));
					//DirectX::XMMATRIX scaled = DirectX::XMLoadFloat4x4(&(vrMan->leftController.obj->position));
					////DirectX::XMMATRIX scaled = DirectX::XMLoadFloat4x4(&obj->position);
					//
					//obj->position = VRManager::leftController.obj->position;
					//obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(obj->position._31 * 3.0f, obj->position._32 * 3.0f, obj->position._33 * 3.0f);
					input = leftAttack;
					amount = 1.0f;
				} else {
					//DirectX::XMFLOAT4 temp(vrMan->rightController.pose._41, vrMan->rightController.pose._42, vrMan->rightController.pose._43, vrMan->rightController.pose._44);
					//Object* obj;
					//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, temp, &obj));
					//DirectX::XMMATRIX scaled = DirectX::XMLoadFloat4x4(&(vrMan->rightController.obj->position));
					////DirectX::XMMATRIX scaled = DirectX::XMLoadFloat4x4(&obj->position);
					//
					//XMStoreFloat4x4(&obj->position, /*DirectX::XMMatrixScaling(0.30f, 0.30f, 0.30f)*/ scaled);
					//obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(obj->position._31 * 3.0f, obj->position._32 * 3.0f, obj->position._33 * 3.0f);
					input = rightAttack;
					amount = 1.0f;
				}
				break;
			}
			break;
		}
		case vr::VREvent_ButtonUnpress:
		{
			switch(event.data.controller.button) {
			case vr::k_EButton_ApplicationMenu:
				break;
			case vr::k_EButton_Grip:
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				break;
			case vr::k_EButton_SteamVR_Trigger:
				if(event.trackedDeviceIndex == vrMan->leftController.index) {
					input = leftAttack;
					amount = 0.0f;
				} else if(event.trackedDeviceIndex == vrMan->rightController.index) {
					input = rightAttack;
					amount = 0.0f;
				}
				break;
			}
			break;
		}
		}
	}
	InputPackage message(input, amount);

	return message;
}

//Keyboard
InputManager::KeyboardInput::KeyboardInput() {
	MapKey(forward, 'W');
	MapKey(backward, 'S');
	MapKey(left, 'A');
	MapKey(right, 'D');
	MapKey(teleport, 'T');
	MapKey(leftAttack, 'B');
	MapKey(rightAttack, 'N');
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	Control input = none;
	float amount = 0;

	while(inputQueue.size() > 0) {
		int j = 0;
		for(auto value : keyBind) {
			if(value.second == inputQueue.front()) {
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

	if(input.control != Control::none)
		MessageEvents::SendMessage(EVENT_Input, InputMessage(input.control, input.amount));

	return input;
}
void InputManager::SetInputType(InputType type) {
	if(bridge)
		delete bridge;
	inputType = type;
	switch(type) {
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
