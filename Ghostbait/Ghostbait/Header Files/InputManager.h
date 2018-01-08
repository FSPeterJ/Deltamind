#pragma once
enum InputType {
	VR,
	KEYBOARD,
	CONTROLLER
};

class InputManager
{
private :
	class Bridge {

	};
	struct Button {
		bool isDown;
	};
	Button forward;
	Button backward;
	Button left;
	Button right;
	Button shoot;
	Button menu;

public:
	InputManager();
	~InputManager();

	bool GetInput();
	InputType GetInputType();
	bool SetInputType(InputType type);
};





