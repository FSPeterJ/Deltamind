#pragma once
#include <unordered_map>
#include <assert.h>
#include "MessageEvents.h"
#include "Messagebox.h"

enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage {
	InputPackage() {}
	Control control;
	float amount;
};

class InputManager {
private:
	struct InputBridge {
		std::unordered_map<Control, int> keyBind;
		virtual bool MapKey(Control control, int key) { return false; };
		virtual InputPackage CheckForInput() { return InputPackage(); };
	};

	struct VRInput: public InputBridge {
		VRInput();
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	};

	struct KeyboardInput: public InputBridge {
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	public:
		KeyboardInput();
	};

	struct ControllerInput: public InputBridge {
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	public:
		ControllerInput();
	};

	static InputType inputType;
	static InputBridge bridge;

public:
	InputManager() {};
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
