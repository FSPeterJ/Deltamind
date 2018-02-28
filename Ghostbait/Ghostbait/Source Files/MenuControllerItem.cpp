#include "MenuControllerItem.h"
#include "PhysicsExtension.h"
#include "Menu.h"


MenuControllerItem::MenuControllerItem() {

}

void MenuControllerItem::UpdateRay() {
	GameObject* temp = nullptr;
	if (Raycast(DirectX::XMFLOAT3(GetPosition()._41, GetPosition()._42, GetPosition()._43 ), DirectX::XMFLOAT3(GetPosition()._31, GetPosition()._32, GetPosition()._33), nullptr, (&temp), 4)) {
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