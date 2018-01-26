#pragma once
#include "StdHeader.h"
#include "DirectXMath.h"

#undef GetObject

enum Control {
	none,
	forward,
	backward,
	left,
	right,
	teleport,
	menu,
	rightItem1,
	rightItem2,
	rightItem3,
	rightItem4,
	rightAttack,
	leftItem1,
	leftItem2,
	leftItem3,
	leftItem4,
	leftAttack,
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

class Object;

class InstantiateMessage: public EventMessageBase {
	PrefabId pid;
	Object** obj;
	DirectX::XMFLOAT4X4 position;
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT3 _position, Object**  _obj = nullptr) : pid(_pid), obj(_obj) { DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z)); }
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT4X4 _position, Object**  _obj = nullptr) : pid(_pid), position(_position), obj(_obj) {}


	const PrefabId GetId() const { return pid; }
	Object ** GetReturnObject() const { return obj; }
	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
	void SetReturnObject(Object * _object) const {
		if(obj != nullptr) {
			*obj = _object;
		}
	}
};

class DestroyMessage: public EventMessageBase {
	Object* obj;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="DestroyMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	DestroyMessage(Object* _obj) : obj(_obj) {}

	Object* RetrieveObject() const { return obj; }
};

class NewObjectMessage: public EventMessageBase {
	Object* obj;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="NewObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	NewObjectMessage(Object* _obj) : obj(_obj) {}

	Object* RetrieveObject() const { return obj; }
};
