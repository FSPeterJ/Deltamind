#pragma once
#include <map>
#include <assert.h>

#define CRASH assert(1 == 0)

enum Control {
	none,
	forward,
	backward,
	left,
	right,
	attack,
	teleport,
	menu
};
enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};
struct InputPackage {
	Control control;
	float amount;
};

class InputManager
{
private:
	struct InputBridge {
		std::map<Control, int> keyBind;
		virtual bool MapKey(Control control, int key) {};
		virtual InputPackage CheckForInput() {};
	};
	struct VRInput : public InputBridge {
		VRInput();
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	};
	struct KeyboardInput : public InputBridge {
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	public:
		KeyboardInput();
	};
	struct ControllerInput : public InputBridge {
		bool MapKey(Control control, int key) override;
		InputPackage CheckForInput() override;
	public:
		ControllerInput();
	};

	InputType inputType = VR;
	InputBridge bridge = VRInput();

public:
	InputManager() {};
	~InputManager() {};
	
	//Purpose:
	//	Called to check input devices for new user input. 
	//	Should be called every frame.
	//	Results in a message sent with respective input
	//Returns:
	//	The control currently selected
	InputPackage HandleInput();
	//Purpose:
	//	Retreive the current input method (Ex. VR, Keyboard, Controller, etc.)
	//Returns:	
	//	The current input method
	inline InputType GetInputType() {return inputType;};
	//Purpose:
	//	Set the current input method (Ex. VR, Keyboard, Controller, etc.)
	//Parameters:	
	//	type - the new inpt method to replace the current one
	void SetInputType(InputType type);
};




