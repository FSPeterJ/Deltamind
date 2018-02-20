#pragma once
#include <queue>
#include <bitset>
#include "MessageStructs.h"  // for Control

enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage;

class VRManager;

class InputManager {
public:
	struct IncomingKey {
		uint64_t key;
		bool isDown;
		IncomingKey() {};
		IncomingKey(uint64_t _key, bool _isDown) : key(_key), isDown(_isDown) {};
	};
private:

	struct InputBridge;
	struct VRInput;
	struct KeyboardInput;
	struct ControllerInput;

	static std::queue<IncomingKey> inputQueue;
	static std::queue<InputPackage> inputPoll;
	static std::bitset<(size_t)Control::Total> keyStates;
	
	InputType inputType = KEYBOARD;
	InputBridge* bridge = nullptr;

	VRManager* vrMan;

public:

	InputManager() {};
	InputManager(InputType type);
	~InputManager();

	static void AddToQueue(IncomingKey key) {
		inputQueue.push(key);
	};

	/// <summary>
	/// Called to check input devices for new user input.
	/// Should be called every frame.
	/// Results in a message sent with respective input.
	/// </summary>
	void HandleInput();

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
