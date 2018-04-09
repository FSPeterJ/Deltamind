#pragma once
#include "GameObject.h"

class ControllerPillar : public GameObject {
	GameObject* lifter = nullptr;
	GameObject* leftController = nullptr;
	GameObject* rightController = nullptr;

	Transform controllerCenter;
	float rot = 0;
	Transform const* playerPos = nullptr;
public:
	ControllerPillar() { SetTag("ControllerPillar"); }
	~ControllerPillar() {}

	void Awake(Object* obj) override;
	//inputToDisplay - string name of the input
	//	ex. "move", "pause", "shoot", "switch", "build"
	void SetType(const char* inputToDisplay);
	void Update() override;
	void Destroy() override;
};