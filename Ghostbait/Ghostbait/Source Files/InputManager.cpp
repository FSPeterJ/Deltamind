#include "InputManager.h"
#include "Messagebox.h"
#include "VRManager.h"    // for VRManager, VRManager::VRController, VRManager::leftController
#include "MessageEvents.h"
#include "Console.h"
#include "WindowsX.h"

#define RAD_PI 3.14159265359

#define RAD_PI_4 0.78539816339
#define RAD_3PI_4 2.35619449019
#define RAD_5PI_4 3.92699081699
#define RAD_7PI_4 5.49778714378

struct InputPackage {
	Control control;
	float amount;
	InputPackage() {};
	InputPackage(const Control _control, const float _amount): control(_control), amount(_amount) {}
};

struct InputManager::InputBridge {
	std::unordered_map<uint64_t, Control> keyBind;
	virtual bool MapKey(Control control, uint64_t key) = 0;
	virtual void CheckForInput() = 0;
};
struct InputManager::VRInput: public InputBridge {
	float rightTPX = 0, rightTPY = 0;
	float leftTPX = 0, leftTPY = 0;
	VRInput();
	bool MapKey(Control control, uint64_t key) override;
	void CheckForInput() override;
};
struct InputManager::KeyboardInput: public InputBridge {
	float cursorX = 0, cursorY = 0;
	POINT winCenter;
	RECT winRect;
	KeyboardInput();
	bool MapKey(Control control, uint64_t key);
	void CheckForInput();
};
struct InputManager::ControllerInput: public InputBridge {
	ControllerInput();
	bool MapKey(Control control, uint64_t key);
	void CheckForInput();
};

InputManager::~InputManager() {
	delete bridge;
};
InputManager::InputManager(InputType type) {
	SetInputType(type);
};

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
bool InputManager::VRInput::MapKey(Control control, uint64_t key) {
	if(keyBind.find(key) == keyBind.end()) {
		keyBind[key] = control;
		return true;
	}
	else { return false; }
}
void InputManager::VRInput::CheckForInput() {
	static bool leftTouchpadTouched = false;
	static bool rightTouchpadTouched = false;

	Control input = none;
	float amount = 0;
	vr::VRControllerState_t state;
	vr::VREvent_t event;


	if(leftTouchpadTouched) {
		VRManager::GetInstance().pVRHMD->GetControllerState(VRManager::GetInstance().leftController.index, &state, sizeof(state));
		leftTPX = state.rAxis[0].x;
		leftTPY = state.rAxis[0].y;
	}
	if(rightTouchpadTouched) {
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
					if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
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
						}
						else {
							input = teleportDown;
							amount = 1;
						}
						break;
					case vr::k_EButton_Grip:
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
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
						}
						else {
							x = rightTPX;
							y = rightTPY;
						}
						if(x == 0 && y == 0) break;

						rads = atan2(x, y);
						if(rads < 0) rads += (float)(2 * RAD_PI);
						if(rads >= RAD_PI_4 && rads < RAD_3PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem2 : rightItem2;
							amount = 1;
						}
						else if(rads >= RAD_3PI_4 && rads < RAD_5PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem3 : rightItem3;
							amount = 1;
						}
						else if(rads >= RAD_5PI_4 && rads < RAD_7PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem1 : rightItem1;
							amount = 1;
						}
						else {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem0 : rightItem0;
							amount = 1;
						}
						break;
					case vr::k_EButton_SteamVR_Trigger:
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
							VRManager::GetInstance().Vibrate(ControllerHand::HAND_Left, 5000);
							input = leftAttack;
							amount = 1.0f;
						}
						else {
							VRManager::GetInstance().Vibrate(ControllerHand::HAND_Right, 5000);
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
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
						}
						else {
							input = teleportDown;
							amount = 0.0f;

							//DirectX::XMStoreFloat4x4(&vrMan->world, DirectX::XMLoadFloat4x4(&vrMan->world) * DirectX::XMMatrixTranslation(vrMan->hmdPose._31, 0, vrMan->hmdPose._33));
						}
						break;
					case vr::k_EButton_Grip:
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
							input = leftCyclePrefab;
							amount = 0;
						}
						else {
							input = rightCyclePrefab;
							amount = 0;
						}
						break;
					case vr::k_EButton_SteamVR_Touchpad:
						float x, y, rads;
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
							x = leftTPX;
							y = leftTPY;
						}
						else {
							x = rightTPX;
							y = rightTPY;
						}
						if(x == 0 && y == 0) break;

						rads = atan2(x, y);
						if(rads < 0) rads += (float)(2 * RAD_PI);
						if(rads >= RAD_PI_4 && rads < RAD_3PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem2 : rightItem2;
							amount = 0;
						}
						else if(rads >= RAD_3PI_4 && rads < RAD_5PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem3 : rightItem3;
							amount = 0;
						}
						else if(rads >= RAD_5PI_4 && rads < RAD_7PI_4) {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem1 : rightItem1;
							amount = 0;
						}
						else {
							input = event.trackedDeviceIndex == VRManager::GetInstance().leftController.index ? leftItem0 : rightItem0;
							amount = 0;
						}
						break;
					case vr::k_EButton_SteamVR_Trigger:
						if(event.trackedDeviceIndex == VRManager::GetInstance().leftController.index) {
							input = leftAttack;
							amount = 0.0f;
						}
						else if(event.trackedDeviceIndex == VRManager::GetInstance().rightController.index) {
							input = rightAttack;
							amount = 0.0f;
						}
						break;
				}
				break;
			}
		}

		if(keyStates[input] != amount > 0.0f ? 1 : 0) {
			keyStates[input] = amount > 0.0f ? 1 : 0;
			inputPoll.push(InputPackage(input, amount));
		}
	}
}

//Keyboard
InputManager::KeyboardInput::KeyboardInput() {
	MapKey(menu, VK_ESCAPE);
	MapKey(forward, 'W');
	MapKey(backward, 'S');
	MapKey(left, 'A');
	MapKey(right, 'D');
	MapKey(rightAttack, MK_RBUTTON);
	MapKey(leftAttack, MK_LBUTTON);
	//MapKey(teleportDown, MK_MBUTTON);
	MapKey(leftCyclePrefab, 'Q');
	MapKey(rightCyclePrefab, 'E');

	MapKey(leftItem0, '1');
	MapKey(leftItem1, '2');
	MapKey(leftItem2, '3');
	MapKey(leftItem3, '4');
	MapKey(rightItem0, '5');
	MapKey(rightItem1, '6');
	MapKey(rightItem2, '7');
	MapKey(rightItem3, '8');

	MapKey(TestInputX, 'X');
	MapKey(TestInputU, 'U');
	MapKey(TestInputI, 'I');
	MapKey(TestInputR, 'R');
	MapKey(TestInputO, 'O');
	MapKey(TestInputJ, 'J');
	MapKey(TestInputK, 'K');
	MapKey(TestInputZ, 'Z');
	MapKey(TestInputC, 'C');
	MapKey(TestInputL, 'L');
	MapKey(releaseKey, VK_MENU); // Alt Key

	GetWindowRect(GetActiveWindow(), &winRect);
	cursorX = ((winRect.right - winRect.left) * 0.5f);
	cursorY = ((winRect.bottom - winRect.top) * 0.5f);
	winCenter.x = (int)cursorX;
	winCenter.y = (int)cursorY;

	SetCursorPos(winRect.left + (int)cursorX, winRect.top + (int)cursorY);
	ShowCursor(false);
}
void InputManager::KeyboardInput::CheckForInput() {
	Control input = none;
	float amount = 0;

	while(inputQueue.size() > 0) {
		input = keyBind[inputQueue.front().wParam];
		static int screenX, screenY;
		switch(inputQueue.front().messageType) {
			case MouseDown:
				amount = 1.0f;
				break;
			case MouseUp:
				amount = 0.0f;
				break;
			case MouseMove:
			{
				if(!mouseActivate) {

					bool movedCursor = false;
					POINT newPos;
					GetCursorPos(&newPos);
					if(newPos.x == screenX && newPos.y == screenY) {
						inputQueue.pop();
						continue;
					}
					input = CameraLeftRight;
					amount = (float)GET_X_LPARAM(inputQueue.front().lParam);
					if(fabsf(amount - winCenter.x) >= 300) {
						screenX = winRect.left + winCenter.x;
						cursorX = (float)winCenter.x;
						movedCursor = true;
					}
					else {
						inputPoll.push(InputPackage(input, amount - cursorX));
						cursorX = amount;
						screenX = newPos.x;
					}
					input = CameraUpDown;
					amount = (float)GET_Y_LPARAM(inputQueue.front().lParam);
					if(fabsf(amount - winCenter.y) >= 300) {
						screenY = winRect.top + winCenter.y;
						cursorY = (float)winCenter.y;
						movedCursor = true;
					}
					else {
						inputPoll.push(InputPackage(input, amount - cursorY));
						cursorY = amount;
						screenY = newPos.y;
					}
					inputQueue.pop();
					if(movedCursor) SetCursorPos(screenX, screenY);
				}
				else {
					inputQueue.pop();
				}
				continue;
			} break;
			case MouseWheel:
				break;
			case KeyboardDown:
				switch(input) {
					case menu:
					{

						MessageEvents::SendMessage(EVENT_GamePause, EventMessageBase());
						break;
					}
					case releaseKey:
					{

						if(!mouseActivate) {
							mouseActivate = true;
							ShowCursor(true);

						}
						else {
							//There is still a case where disable mouse camera control, moving the mouse, and then re-enabling it causes a camera jerk
							mouseActivate = false;
							ShowCursor(false);
							POINT newPos;
							GetCursorPos(&newPos);
							screenX = newPos.x;
							screenY = newPos.y;
							cursorY = 0;
							cursorX = 0;
						}
						break;
					}
				}

				amount = 1.0f;
				break;
			case KeyboardUp:
				/*switch(input) {
					case releaseKey:
					{

						if(mouseActivate) {
							mouseActivate = false;
							ShowCursor(false);
						}
						break;
					}
				}*/
				amount = 0.0f;
				break;
			default:
				break;
		}

		if(keyStates[input] != (amount > 0.0f ? 1 : 0)) {
			keyStates[input] = amount > 0.0f ? 1 : 0;
			inputPoll.push(InputPackage(input, amount));
		}
		inputQueue.pop();
	}
}
bool InputManager::KeyboardInput::MapKey(Control control, uint64_t key) {
	if(keyBind.find(key) == keyBind.end()) {
		keyBind[key] = control;
		return true;
	}
	else { return false; };
}

//Controller
InputManager::ControllerInput::ControllerInput() {}
void InputManager::ControllerInput::CheckForInput() {

	inputPoll.push(InputPackage(none, 1));
}
bool InputManager::ControllerInput::MapKey(Control control, uint64_t key) {
	if(keyBind.find(key) == keyBind.end()) {
		keyBind[key] = control;
		return true;
	}
	else { return false; };
}

//Input Manager
std::queue<InputManager::IncomingMessage> InputManager::inputQueue;
std::queue<InputPackage> InputManager::inputPoll;
std::bitset<(size_t)Control::Total> InputManager::keyStates;
bool InputManager::mouseActivate = false;
InputType InputManager::inputType = KEYBOARD;

void InputManager::HandleInput() {
	bridge->CheckForInput();

	while(inputPoll.size() > 0) {
		MessageEvents::SendMessage(EVENT_Input, InputMessage(inputPoll.front().control, inputPoll.front().amount));
		inputPoll.pop();
	}
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
