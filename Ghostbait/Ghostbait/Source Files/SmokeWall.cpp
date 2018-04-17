#include "SmokeWall.h"
#include "RandomEngine.h"
#include "Material.h"
#include "ScrollingUVManager.h"

SmokeWall::SmokeWall() {
	SetTag("SmokeWall");
}

void SmokeWall::Awake(Object* obj) {
	GameObject::Awake(obj);
	UnRender();
	RenderTransparent();
	switch (Omiracon::Random::RandomNumber(1, 4)) {
		case 1:
			SwapComponentVarient<Material>("1");
			break;
		case 2:
			SwapComponentVarient<Material>("1");
			break;
		case 3:
			SwapComponentVarient<Material>("1");
			break;
	}
}

void SmokeWall::Destroy() {
	UnRender();
	GameObject::Destroy();
}