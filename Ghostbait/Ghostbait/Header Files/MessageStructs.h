#pragma once
#include "StdHeader.h"
#include "DirectXMath.h"

#undef GetObject

class ObjectFactory;
class GameData;
class Core;
class Player;
//Please do this next time
typedef unsigned long		AkUInt32;
typedef AkUInt32			AkUniqueID;


class GameObject;

class ComponentBase;

enum Control {
	none,
	forward,
	backward,
	left,
	right,
	teleportDown,
	teleportUp,
	pause,
	releaseKey,

	rightItem0,
	rightItem1,
	rightItem2,
	rightItem3,
	rightAttack,
	rightTouch,
	rightCyclePrefab,

	leftItem0,
	leftItem1,
	leftItem2,
	leftItem3,
	leftAttack,
	leftTouch,
	leftCyclePrefab,

	TestInputU,
	TestInputI,
	TestInputO,
	TestInputJ,
	TestInputK,
	TestInputL,
	TestInputX,
	TestInputR,
	TestInputV,
	TestInputB,
	TestInputN,


	GodMode,
	Sprint,
	Crouch,

	CameraLeftRight,
	CameraUpDown,
	LeftAction,
	RightAction,

	Total,
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

class InstantiateMessage: public EventMessageBase {
	friend ObjectFactory;
	PrefabId pid;
	DirectX::XMFLOAT4X4 position;
public:
	GameObject** obj = nullptr;

	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateTypeMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT3 _position, GameObject**  _obj = nullptr): pid(_pid), obj(_obj) { DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z)); }
	InstantiateMessage(const PrefabId _pid, const DirectX::XMFLOAT4X4 _position, GameObject**  _obj = nullptr): pid(_pid), position(_position), obj(_obj) {}

	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
	PrefabId GetPrefabId() { return pid; };
};

template <typename ObjectType>
class InstantiateTypeMessage: public EventMessageBase {
	friend ObjectFactory;
	PrefabId pid = 0;
	unsigned tid = 0; //Object Type ID
	DirectX::XMFLOAT4X4 position;
public:
	ObjectType** obj = nullptr;

	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateTypeMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateTypeMessage(const PrefabId _pid, const DirectX::XMFLOAT3 _position, ObjectType**  _obj = nullptr): pid(_pid), obj(_obj) {
		DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z));
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}
	InstantiateTypeMessage(const DirectX::XMFLOAT3 _position, ObjectType**  _obj = nullptr): obj(_obj) {
		DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z));
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}
	InstantiateTypeMessage(const PrefabId _pid, const DirectX::XMFLOAT4X4 _position, ObjectType**  _obj = nullptr): pid(_pid), position(_position), obj(_obj) {
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}
	InstantiateTypeMessage(const DirectX::XMFLOAT4X4 _position, ObjectType**  _obj = nullptr): position(_position), obj(_obj) {
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}
	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
	PrefabId GetPrefabId() { return pid; };

};


template <typename ObjectType>
class InstantiateNameMessage: public EventMessageBase {
	friend ObjectFactory;
	DirectX::XMFLOAT4X4 position;
	unsigned tid = 0; //Object Type ID
public:
	char* debug_name;
	ObjectType** obj = nullptr;
	/// <summary>
	/// Initializes a new instance of the <see cref="InstantiateTypeMessage"/> class.
	/// </summary>
	/// <param name="_pid">The object's Prefab id.</param>
	/// <param name="_position">Where to instantiate</param>
	/// <param name="_obj"> return pointer reference of the object (optional)</param>
	InstantiateNameMessage(char* const name_Debug, const DirectX::XMFLOAT3 _position, ObjectType**  _obj = nullptr): debug_name(name_Debug), obj(_obj) {
		DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z));
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}
	InstantiateNameMessage(char* const name_Debug, const DirectX::XMFLOAT4X4 _position, ObjectType**  _obj = nullptr): debug_name(name_Debug), position(_position), obj(_obj) {
		tid = TypeMap::GetObjectTypeID<ObjectType>();
	}

	DirectX::XMFLOAT4X4 GetPosition() const { return position; }
};

class StandardObjectMessage: public EventMessageBase {
	GameObject* obj = nullptr;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="StandardObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	StandardObjectMessage(GameObject* _obj): obj(_obj) {}

	GameObject* RetrieveObject() const { return obj; }
};

class SpawnerCreatedMessage: public EventMessageBase {
	GameObject* obj = nullptr;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="StandardObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	SpawnerCreatedMessage(GameObject* _obj): obj(_obj) {}

	GameObject* RetrieveObject() const { return obj; }
};

class SnapMessage: public EventMessageBase {
public:
	DirectX::XMFLOAT2* position;
	SnapMessage(DirectX::XMFLOAT2* _position): position(_position) {}
};

class GameDataMessage : public EventMessageBase {
	GameData const** data = nullptr;
public:
	GameDataMessage(GameData const** _gameData) :data(_gameData) {}
	GameData const** RetrieveData() const { return data; }
};

class GetPlayerMessage : public EventMessageBase {
	Player const** data = nullptr;
public:
	GetPlayerMessage(Player const** _playerPos) :data(_playerPos) {}
	Player const** RetrieveData() const { return data; }
};

class CoreMessage : public EventMessageBase {
	Core const** data = nullptr;
public:
	CoreMessage(Core const** _data) : data(_data) {}
	Core const** RetrieveData() const { return data; }
};

class ChangeSceneMessage : public EventMessageBase {
	const char* sceneName = nullptr;
public:
	ChangeSceneMessage(const char* _sceneName) : sceneName(_sceneName) {}
	const char* RetrieveData() const { return sceneName; }
};

class ObjectMessage: public EventMessageBase {
public:
	GameObject* obj = nullptr;

	/// <summary>
	/// Initializes a new instance of the <see cref="ObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	ObjectMessage(GameObject* _obj): obj(_obj) {}

	GameObject* RetrieveObject() const { return obj; }
};

class NewEmitterMessage: public EventMessageBase {
public:
	ComponentBase** emit = nullptr;
	unsigned EmitterID;
	const DirectX::XMFLOAT3* position;
	DirectX::XMFLOAT3 force;
	/// <summary>
	/// Initializes a new instance of the <see cref="ObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	NewEmitterMessage(const DirectX::XMFLOAT3* _position, unsigned _emitterID, const DirectX::XMFLOAT3 _force = DirectX::XMFLOAT3(0,0,0),  ComponentBase** _obj = nullptr): emit(_obj), EmitterID(_emitterID), force(_force), position(_position){}

};

class DeleteEmitterMessage: public EventMessageBase {
public:
	ComponentBase* emit = nullptr;
	/// <summary>
	/// Initializes a new instance of the <see cref="ObjectMessage"/> class.
	/// </summary>
	/// <param name="_obj">The object.</param>
	DeleteEmitterMessage(ComponentBase* _obj = nullptr): emit(_obj) {}

};


class SoundRequestMessage: public EventMessageBase {
	GameObject* obj = nullptr;
	AkUniqueID sound;
public:
	SoundRequestMessage(GameObject* _obj, AkUniqueID _sound): obj(_obj), sound(_sound) {}
	GameObject* RetrieveObject() const { return obj; }
	AkUniqueID RetrieveSound() const { return sound; }
};

class StartEventMessage : public EventMessageBase {
	std::string levelName = "";
public:
	StartEventMessage(std::string _levelName) : levelName(_levelName) {}
	std::string RetrieveLevelName() const { return levelName; }
};
