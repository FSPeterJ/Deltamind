#pragma once
#include "GameObject.h"

class CoreShield : public GameObject {
	void Update();
public:
	void OnCollision(GameObject* other);

	void Enable() override {
		UnRender();
		RenderTransparent();
		GameObject::Enable();
	}

	void Destroy() {
		GameObject::Destroy();
	};
};