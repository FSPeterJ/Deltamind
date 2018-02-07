#pragma once
#include "StdHeader.h"
#include "DirectXMath.h" //todo get rid

#undef GetObject

class GameObject;

enum Control {
	none,
	forward,
	backward,
	left,
	right,
	teleportDown,
	teleportUp,
	menu,
	rightItem1,
	rightItem2,
	rightItem3,
	rightItem4,
	rightAttack,
	rightTouch,
	leftItem1,
	leftItem2,
	leftItem3,
	leftItem4,
	leftAttack,
	leftTouch,
	TestInputU,
	TestInputI,
	TestInputO,
	TestInputJ,
	TestInputK,
	TestInputL,
	TestInputX,
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
	InputMessage(const Control _ctrl, const float _amount) : ctrl(_ctrl), amount(_amount) {}

	const float GetAmount() const { return amount; }
	const Control GetControl() const { return ctrl; }
};

class InstantiateMessage: public EventMessageBase {
	PrefabId pid;
	DirectX::XMFLOAT4X4 position;
public:
	GameObject * * obj;

	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT3 _position, GameObject**  _obj = nullptr) : pid(_pid), obj(_obj) { DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z)); }
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT4X4 _position, GameObject**  _obj = nullptr) : pid(_pid), position(_position), obj(_obj) {}

	PrefabId GetPrefabId() const { return pid; }
	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
};

class InstantiateNameMessage: public EventMessageBase {
	DirectX::XMFLOAT4X4 position;
public:
	char* debug_name;
	GameObject * * obj;
	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateNameMessage(char* const name_Debug, const DirectX::XMFLOAT3 _position, GameObject**  _obj = nullptr) : debug_name(name_Debug), obj(_obj) { DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z)); }
	InstantiateNameMessage(char* const name_Debug, const DirectX::XMFLOAT4X4 _position, GameObject**  _obj = nullptr) : debug_name(name_Debug), position(_position), obj(_obj) {}

	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
};

class DestroyMessage: public EventMessageBase {
	GameObject* obj;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="DestroyMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	DestroyMessage(GameObject* _obj) : obj(_obj) {}

	GameObject* RetrieveObject() const { return obj; }
};

//Duplicate is unnessessary
class NewObjectMessage: public EventMessageBase {
	GameObject* obj;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="NewObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	NewObjectMessage(GameObject* _obj) : obj(_obj) {}

	GameObject* RetrieveObject() const { return obj; }
};
