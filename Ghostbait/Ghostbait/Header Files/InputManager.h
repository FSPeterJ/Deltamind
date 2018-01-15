#pragma once
#include "MessageEvents.h"
#include "VRManager.h"

enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage {
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

	struct InputBridge : Input {
		std::unordered_map<Control, int> keyBind;
		bool MapKey(Control control, int key) { return false; }
		InputPackage CheckForInput() { return InputPackage(none, 0.0f); }
	};

	struct VRInput: public InputBridge {
		VRInput();
		bool MapKey(Control control, int key);
		InputPackage CheckForInput();
	};

	struct KeyboardInput: public InputBridge {
		bool MapKey(Control control, int key);
		InputPackage CheckForInput();
	public:
		KeyboardInput();
	};

	struct ControllerInput: public InputBridge {
		bool MapKey(Control control, int key);
		InputPackage CheckForInput();
	public:
		ControllerInput();
	};

	static InputType inputType;
	static InputBridge bridge;

	VRManager* vrMan;

public:
	InputManager() {};
	InputManager(VRManager* vrManager) { vrMan = vrManager; };
	~InputManager() {};

	/// <summary>
	/// Called to check input devices for new user input.
	/// Should be called every frame.
	/// Results in a message sent with respective input.
	/// </summary>
	/// <returns>The control currently selected.</returns>
	static InputPackage HandleInput();

	/// <summary>
	/// Retreive the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <returns>The current input method.</returns>
	static inline InputType GetInputType() { return inputType; };

	/// <summary>
	/// Sets the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <param name="type">The new input method to replace the current one.</param>
	static void SetInputType(InputType type);
};
