#pragma once
#include "Object.h"
#include "Controlable.h"

#include "Console.h"

class GameObject: public Object {
	friend class ObjectFactory;
	virtual void GameObject::SetData(char* data) {};
protected:
	std::string tag = "none";
	unsigned typeID = 0;
	unsigned updateID = 0;  //Update Delegate ID
	unsigned eventDeleteAllGameObjects = 0;

	bool enabled = false;
	bool destroyOnReset = true;

public:
	GameObject();
	void DisableNow();

	virtual void Awake();
	virtual void Subscribe();
	virtual void UnSubscribe();
	virtual void Enable(bool _destroyOnReset = true);
	virtual void Disable();
	virtual void Update();
	virtual void Destroy();


	virtual void OnCollision(GameObject* obj);
	virtual void OnTrigger(GameObject* obj);

	inline const std::string GetTag() const { return tag; };
	inline void SetTag(std::string _tag) { tag = _tag; };
};

//Other
class MenuCube: public GameObject {
	void Update();
public:
	void OnCollision(GameObject* other);

	void Enable(bool _destroyOnReset = true) override{
		GameObject::Enable(_destroyOnReset);
	}

	void Destroy() {
		GameObject::Destroy();
	};
};
class CoreCube: public GameObject {
	bool enemyTouched = false;
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other);
	void Destroy() override;
};
