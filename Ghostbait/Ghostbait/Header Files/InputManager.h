#pragma once
#include <queue>
#include <bitset>
#include "MessageStructs.h"  // for Control
#include "CheatCode.h"

enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

struct InputPackage;

class VRManager;

class InputManager {
public:
	enum MessageType {
		Invalid,
		KeyboardUp,
		KeyboardDown,
		MouseUp,
		MouseDown,
		MouseWheel,
		MouseMove,
	};
	struct IncomingMessage {
		uint64_t wParam;
		int64_t lParam;
		MessageType messageType;
		IncomingMessage() {};
		IncomingMessage(uint64_t _wParam, int64_t _lParam, MessageType _messType) : wParam(_wParam), lParam(_lParam), messageType(_messType) {};
	};
private:

	struct InputBridge;
	struct VRInput;
	struct KeyboardInput;
	struct ControllerInput;

	static std::queue<IncomingMessage> inputQueue;
	static std::queue<InputPackage> inputPoll;
	static std::bitset<(size_t)Control::Total> keyStates;
	
	static InputType inputType;
	InputBridge* bridge = nullptr;

	VRManager* vrMan;

	static bool mouseActivate;

	static CheatCode godMode;
	static CheatCode freeMoney;
	static CheatCode smite;
	static CheatCode timeScale;

public:

	InputManager() {};
	InputManager(InputType type);
	~InputManager();

	static void AddToQueue(IncomingMessage message) {
		if(inputType != VR) inputQueue.push(message);
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
	inline InputType GetInputType() { return inputType; };

	/// <summary>
	/// Sets the current input method (Ex. VR, Keyboard, Controller, etc.)
	/// </summary>
	/// <param name="type">The new input method to replace the current one.</param>
	void SetInputType(InputType type);
};
