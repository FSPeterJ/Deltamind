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
	void Disable();
	void DisableNow();
	virtual void OnCollision(GameObject* obj);

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
class CoreCube: public GameObject {
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other);
};