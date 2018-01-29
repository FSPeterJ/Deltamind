#pragma once
#include "Object.h"
#include "Controlable.h"
#include "Console.h"
#include "GhostTime.h"
//#include <functional>

class GameObject: public Object {
	//Until delegate unsubscribe is fixed
protected:

	std::string tag = "none";
	bool isAwake = false;

public:
	GameObject();

	void Enable();
	virtual void OnCollision(GameObject* obj);
	void Disable();

	void Destroy();

	virtual void Awake() {};
	virtual void Update() {};

	inline const std::string GetTag() const { return tag; };
	inline void SetTag(std::string _tag) { tag = _tag; };
};

//Other
class MenuCube : public GameObject {
public:
	void OnCollision(GameObject* other){
		if (other->GetTag() == "Bullet") {
			Destroy();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 10, 0, 0 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { -10, 0, 0 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, 10 }));
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, -10 }));
			GameObject* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(8/*Core*/, { 0, 1.5f, 0 }, (Object**)&obj));
			DirectX::XMStoreFloat4x4(&obj->position, DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
		}
	};
};
class CoreCube : public GameObject {
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other) {
		if (other->GetTag() == "enemy") {
			Console::WriteLine("YOU LOSE!");
			Debug("YOU LOSE!");
			Destroy();
			Object* temper;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(10/*LoseCube*/, { 0, 0.75, 0 }, &temper));
			DirectX::XMStoreFloat4x4(&temper->position, DirectX::XMLoadFloat4x4(&temper->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));

		}
	};
};