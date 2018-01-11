#pragma once
#include "StdHeader.h"

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
	InputMessage(Control _ctrl, float _amount) : ctrl(_ctrl), amount(_amount) {}

	 const float GetAmount() const { return amount; }
	 const Control GetControl() const { return ctrl; }
};

class InstantiateMessage: public EventMessageBase {
	PrefabId obj;
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object's prefab id.</param>
	InstantiateMessage(PrefabId _obj) : obj(_obj) {}

	const PrefabId GetId() const { return obj; }
};

class DestroyMessage: public EventMessageBase {
	Object* obj;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="DestroyMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	DestroyMessage(Object* _obj) : obj(_obj) {}

	Object* GetObject() const { return obj; }
};
