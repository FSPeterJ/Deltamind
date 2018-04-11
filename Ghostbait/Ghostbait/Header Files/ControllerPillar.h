#pragma once
#include "GameObject.h"

class Player;

class ControllerPillar : public GameObject {
protected:
	enum Type {
		Default,
		Move,
		Pause,
		Shoot,
		SwitchItem,
		CycleBuild,
	};

	Type type;

	GameObject* lifter = nullptr;
	GameObject* leftDisplay = nullptr;
	GameObject* rightDisplay = nullptr;
	GameObject* centerDisplay = nullptr;
	GameObject* topDisplay = nullptr;
	GameObject* bottomDisplay = nullptr;

	bool isVR = true;

	Transform displayCenter;
	float rot = 0;
	Player const* player = nullptr;
public:
	ControllerPillar() { SetTag("ControllerPillar"); }
	~ControllerPillar() {}

	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
};

class ControllerPillar_Move : public ControllerPillar {
	void Awake(Object* obj) override;
};

class ControllerPillar_Build : public ControllerPillar {
	void Awake(Object* obj) override {
		ControllerPillar::Awake(obj);
	}
};
class ControllerPillar_Pause : public ControllerPillar {
	void Awake(Object* obj) override;
};
class ControllerPillar_Shoot : public ControllerPillar {
	void Awake(Object* obj) override {
		ControllerPillar::Awake(obj);
	}
};
class ControllerPillar_Switch : public ControllerPillar {
	void Awake(Object* obj) override;
};