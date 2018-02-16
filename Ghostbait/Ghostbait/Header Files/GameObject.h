#pragma once
#include "Object.h"
#include "Controlable.h"

class GameObject: public Object {
	friend class ObjectFactory;
	virtual void GameObject::SetData(char* data) {};
protected:
	std::string tag = "none";
	unsigned typeID = 0;
	unsigned updateID = 0;  //Update Delegate ID

	virtual void RestartGame() {};

public:
	GameObject();

	void Enable();
	void Disable();
	void DisableNow();
	virtual void OnCollision(GameObject* obj);

	void Destroy();

	virtual void Awake() {};
	virtual void Update() {};

	inline const std::string GetTag() const { return tag; };
	inline void SetTag(std::string _tag) { tag = _tag; };
	virtual void PassObject(GameObject* obj, char* tag);
};

//Other
class MenuCube: public GameObject {
	void Update();
public:
	void OnCollision(GameObject* other);
};
class CoreCube: public GameObject {
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other);
};
