#pragma once
#include "MessageEvents.h"
#include "VRManager.h"

enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage {
	InputPackage() {};
	InputPackage(Control _control, float _amount) : control(_control), amount(_amount) {}
	Control control;
	float amount;
};

class InputManager {
private:
	struct Input {
		virtual bool MapKey(Control control, int key) = 0;
		virtual InputPackage CheckForInput() = 0;
	};

	struct InputBridge {// : Input {
		std::unordered_map<Control, int> keyBind;
		virtual bool MapKey(Control control, int key) = 0;// { return false; };// { return false; }
		virtual InputPackage CheckForInput() = 0;// { return InputPackage(none, 0); };// { return InputPackage(none, 0.0f); }
	};

	struct VRInput: public InputBridge {
		VRManager* vrMan;
		VRInput() {};
		VRInput(VRManager* vrManager);
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;

	};

	struct KeyboardInput: public InputBridge {
		KeyboardInput();
		bool MapKey(Control control, int key);
		InputPackage CheckForInput();
	};

	struct ControllerInput: public InputBridge {
		ControllerInput();
		bool MapKey(Control control, int key);
		InputPackage CheckForInput();
	};

	InputType inputType = KEYBOARD;
	InputBridge* bridge = nullptr;

	VRManager* vrMan;

public:
	InputManager() { };
	InputManager(InputType type, VRManager* vrManager = nullptr) { SetInputType(type); vrMan = vrManager; };
	~InputManager() {};

	/// <summary>
	/// Called to check input devices for new user input.
	/// Should be called every frame.
	/// Results in a message sent with respective input.
	/// </summary>
	/// <returns>The control currently selected.</returns>
	InputPackage HandleInput();

	/// <summary>
	/// Retreive the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <returns>The current input method.</returns>
	inline InputType GetInputType() { return inputType; };

	/// <summary>
	/// Sets the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <param name="type">The new input method to replace the current one.</param>
	void SetInputType(InputType type);
};
