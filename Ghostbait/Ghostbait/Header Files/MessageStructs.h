#pragma once


enum Control {
	NONE,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	ATTACK,
	TELEPORT,
	MENU
};

class EventMessageBase {
public:
	EventMessageBase() {};
	~EventMessageBase() {};
};

class InputMessage: public EventMessageBase {
	Control ctrl;
	float amount;
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="InputMessage"/> class.
	/// </summary>
	/// <param name="_ctrl">The control.</param>
	/// <param name="_amount">The amount.</param>
	InputMessage(const Control _ctrl, const float _amount): ctrl(_ctrl), amount(_amount) {}

	const float GetAmount() const { return amount; }
	const Control GetControl() const { return ctrl; }
};
