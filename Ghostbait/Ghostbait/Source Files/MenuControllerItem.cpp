#include "MenuControllerItem.h"
#include "PhysicsExtension.h"
#include "Menu.h"
#include "MessageEvents.h"


MenuControllerItem::MenuControllerItem() {

}

void MenuControllerItem::UpdateRay() {
	GameObject* temp = nullptr;
	if (Raycast(source, DirectX::XMFLOAT3(source->GetMatrix()._31, source->GetMatrix()._32, source->GetMatrix()._33), nullptr, (&temp), &ray, 10, "Button")) {
		MenuOption* newMenuItem = dynamic_cast<MenuOption*>(temp);
		//Did we collide with an option?
		if (newMenuItem) {
			//If option is same as previous, continue;
			if (newMenuItem == currentMenuItem) return;
			//else unhighlight the old (current), highlight the new, and reset current to new;
			if (currentMenuItem) currentMenuItem->UnHighlight();
			newMenuItem->Highlight();
			currentMenuItem = newMenuItem;
		}
	}
	else if (currentMenuItem) {
		currentMenuItem->UnHighlight();
		currentMenuItem = nullptr;
	}
}
void MenuControllerItem::Activate() {
	if (currentMenuItem) {
		currentMenuItem->Select();
	}
}

void MenuControllerItem::Render() {
	GameObject::RenderToFront();
	ray.Create(true);
}

void MenuControllerItem::UnRender() {
	GameObject::UnRender();
	ray.Destroy();
}


void MenuControllerItem::Awake(Object* obj) {
	ray.SetFile("Assets/Ray.ghost");
	GameObject::Awake(obj);
}