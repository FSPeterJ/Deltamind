#pragma once
#include "GameObject.h"

class SSLogo : public GameObject {
	GameObject* text;
public:
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
};