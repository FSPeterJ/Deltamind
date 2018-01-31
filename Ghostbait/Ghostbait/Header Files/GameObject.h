#pragma once
#include "Object.h"
#include "Controlable.h"

class GameObject: public Object {
	//Until delegate unsubscribe is fixed

protected:
	
	std::string tag = "none";
	unsigned updateID;

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
	void Update();
public:
	void OnCollision(GameObject* other);;
};
class CoreCube : public GameObject {
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other) {
		if (other->GetTag() == "enemy") {
			Console::WriteLine <<"YOU LOSE!";
			Console::OutLine << "YOU LOSE!";
			Destroy();
			Object* temper;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(10/*LoseCube*/, { 0, 0.75, 0 }, &temper));
			DirectX::XMStoreFloat4x4(&temper->position, DirectX::XMLoadFloat4x4(&temper->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
		}
	};
};
