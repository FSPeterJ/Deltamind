#pragma once
#include <queue>
enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage;

class VRManager;

class InputManager {
private:

	struct InputBridge;
	struct VRInput;
	struct KeyboardInput;
	struct ControllerInput;

	static std::queue<uint64_t> inputQueue;

	InputType inputType = KEYBOARD;
	InputBridge* bridge = nullptr;

	VRManager* vrMan;
	InputPackage* inputPoll;

public:
	InputManager() {};
	InputManager(InputType type);
	~InputManager();

	static void AddToQueue(uint64_t key) { inputQueue.push(key); };

	/// <summary>
	/// Called to check input devices for new user input.
	/// Should be called every frame.
	/// Results in a message sent with respective input.
	/// </summary>
	/// <returns>The control currently selected.</returns>
	InputPackage* HandleInput();

	/// <summary>
	/// Retreive the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <returns>The current input method.</returns>
	inline InputType GetInputType();

	/// <summary>
	/// Sets the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <param name="type">The new input method to replace the current one.</param>
	void SetInputType(InputType type);
};
