#include "InputManager.h"
#include "Messagebox.h"
#include "VRManager.h"    // for VRManager, VRManager::VRController, VRManager::leftController
#include "MessageEvents.h"
#include "Console.h"
#include "MessageStructs.h"  // for Control

#define RAD_PI 3.14159265359

#define RAD_PI_4 0.78539816339
#define RAD_3PI_4 2.35619449019
#define RAD_5PI_4 3.92699081699
#define RAD_7PI_4 5.49778714378

struct InputPackage {
	Control control;
	float amount;
	InputPackage() {};
	InputPackage(const Control _control, const float _amount) : control(_control), amount(_amount) {}
};

struct InputManager::InputBridge {
	std::unordered_map<Control, int> keyBind;
	virtual bool MapKey(Control control, int key) = 0;
	virtual InputPackage CheckForInput() = 0;
};
struct InputManager::VRInput: public InputBridge {
	float rightTPX = 0, rightTPY = 0;
	float leftTPX = 0, leftTPY = 0;
	VRInput();
	bool MapKey(Control control, int key) override;
	InputPackage CheckForInput() override;
};
struct InputManager::KeyboardInput: public InputBridge {
	KeyboardInput();
	bool MapKey(Control control, int key);
	InputPackage CheckForInput();
};
struct InputManager::ControllerInput: public InputBridge {
	ControllerInput();
	bool MapKey(Control control, int key);
	InputPackage CheckForInput();
};

InputManager::~InputManager() {
	delete bridge; delete inputPoll;
};
InputManager::InputManager(InputType type) {
	SetInputType(type);
	inputPoll = new InputPackage();
};
inline InputType InputManager::GetInputType() { return inputType; };

//VR
InputManager::VRInput::VRInput() {
	MapKey(none, 0);
	MapKey(forward, 1);
	MapKey(backward, 2);
	MapKey(left, 3);
	MapKey(right, 4);
	MapKey(teleportDown, 5);
	MapKey(teleportUp, 6);
	MapKey(leftAttack, 7);
	MapKey(rightAttack, 8);
	MapKey(menu, 9);
	MapKey(leftTouch, 10);
	MapKey(rightTouch, 11);

}
bool InputManager::VRInput::MapKey(Control control, int key) {
	if(keyBind.find(control) != keyBind.end()) {
		keyBind[control] = key;
		return true;
	} else { return false; }
}
InputPackage InputManager::VRInput::CheckForInput() {
	static bool leftTouchpadTouched = false;
	static bool rightTouchpadTouched = false;

	Control input = none;
	float amount = 0;
	vr::VRControllerState_t state;
	vr::VREvent_t event;
	
	
	if (leftTouchpadTouched) {
		VRManager::GetInstance().pVRHMD->GetControllerState(VRManager::GetInstance().leftController.index, &state, sizeof(state));
		leftTPX = state.rAxis[0].x;
		leftTPY = state.rAxis[0].y;
	}
	if (rightTouchpadTouched) {
		VRManager::GetInstance().pVRHMD->GetControllerState(VRManager::GetInstance().rightController.index, &state, sizeof(state));
		rightTPX = state.rAxis[0].x;
		rightTPY = state.rAxis[0].y;
	}

	while(VRManager::GetInstance().pVRHMD->PollNextEvent(&event, sizeof(event))) {
		switch(event.eventType) {
		case vr::VREvent_ButtonTouch:
		{
			if(event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
				if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					leftTouchpadTouched = true;
					VRManager::GetInstance().pVRHMD->GetControllerState(VRManager::GetInstance().leftController.index, &state, sizeof(state));
					leftTPX = state.rAxis[0].x;
					leftTPY = state.rAxis[0].y;
					Console::WriteLine << "Touched!!";
					input = leftTouch;
					amount = 1.0f;
				} 
				else {
					rightTouchpadTouched = true;
					VRManager::GetInstance().pVRHMD->GetControllerState(VRManager::GetInstance().rightController.index, &state, sizeof(state));
					rightTPX = state.rAxis[0].x;
					rightTPY = state.rAxis[0].y;
					input = rightTouch;
					amount = 1.0f;
				}
			}
			break;
		}
		case vr::VREvent_ButtonUntouch:
		{
			if(event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
				if (event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					leftTouchpadTouched = false;
					input = leftTouch;
					amount = 0.0f;
				}
				else {
					rightTouchpadTouched = false;
					input = rightTouch;
					amount = 0.0f;
				}
			}
			break;
		}
		case vr::VREvent_ButtonPress:
		{
			switch(event.data.controller.button) {
			case vr::k_EButton_ApplicationMenu:
				if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					MessageEvents::SendMessage(EVENT_GamePause, EventMessageBase());
				} else {
					input = teleportDown;
					amount = 1;
				}
				break;
			case vr::k_EButton_Grip:
				if (event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					input = leftCyclePrefab;
					amount = 1;
				}
				else {
					input = rightCyclePrefab;
					amount = 1;
				}
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				float x, y, rads;
				if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					x = leftTPX;
					y = leftTPY;
				} else {
					x = rightTPX;
					y = rightTPY;
				}
				if(x == 0 && y == 0) break;

				rads = atan2(x, y);
				if(rads < 0) rads += (float) (2 * RAD_PI);
				if(rads >= RAD_PI_4 && rads < RAD_3PI_4) {
					input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem2 : rightItem2;
					amount = 1;
				} else if(rads >= RAD_3PI_4 && rads < RAD_5PI_4) {
					input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem3 : rightItem3;
					amount = 1;
				} else if(rads >= RAD_5PI_4 && rads < RAD_7PI_4) {
					input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem1 : rightItem1;
					amount = 1;
				} else {
					input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem0 : rightItem0;
					amount = 1;
				}
				break;
			case vr::k_EButton_SteamVR_Trigger:
				if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					VRManager::GetInstance().Vibrate(VRControllerType::Left, 500);
					input = leftAttack;
					amount = 1.0f;
				} else {
					VRManager::GetInstance().Vibrate(VRControllerType::Right, 500);
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
				if (event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
				}
				else {
					input = teleportUp;
					amount = 1;
					//DirectX::XMStoreFloat4x4(&vrMan->world, DirectX::XMLoadFloat4x4(&vrMan->world) * DirectX::XMMatrixTranslation(vrMan->hmdPose._31, 0, vrMan->hmdPose._33));
				}
				break;
			case vr::k_EButton_Grip:
				break;
			case vr::k_EButton_SteamVR_Touchpad:
				break;
			case vr::k_EButton_SteamVR_Trigger:
				if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
					input = leftAttack;
					amount = 0.0f;
				} else if(event.trackedDeviceIndex == VRManager::GetInstance().rightController.index) {
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
	MapKey(teleportUp, 'T');
	MapKey(leftAttack, 'Q');
	MapKey(rightAttack, 'E');
	MapKey(TestInputX, 'X');
	MapKey(TestInputU, 'U');
	MapKey(TestInputI, 'I');
	MapKey(TestInputO, 'O');
	MapKey(TestInputJ, 'J');
	MapKey(TestInputK, 'K');
	MapKey(TestInputL, 'L');
}
InputPackage InputManager::KeyboardInput::CheckForInput() {
	Control input = none;
	float amount = 0;

	while(inputQueue.size() > 0) {
		int j = 0;
		for(auto &value : keyBind) {
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
InputManager::ControllerInput::ControllerInput() {}
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

InputPackage* InputManager::HandleInput() {
	*inputPoll = bridge->CheckForInput();

	if(inputPoll->control != Control::none)
		MessageEvents::SendMessage(EVENT_Input, InputMessage(inputPoll->control, inputPoll->amount));

	return inputPoll;
}
void InputManager::SetInputType(InputType type) {
	if(bridge)
		delete bridge;
	inputType = type;
	switch(type) {
	case InputType::VR:
		this->bridge = new VRInput();
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
