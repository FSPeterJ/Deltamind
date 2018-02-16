#include "MenuControllerItem.h"
#include "PhysicsExtension.h"
#include "Menu.h"


MenuControllerItem::MenuControllerItem() {

}

void MenuControllerItem::UpdateRay() {
	if (Raycast(DirectX::XMFLOAT3(position._41, position._42, position._43), DirectX::XMFLOAT3(position._31, position._32, position._33), &DirectX::XMFLOAT3(), const_cast<const GameObject**>(&currentlySelectedObject), 4)) {
		MenuOption* newMenuItem = dynamic_cast<MenuOption*>(currentlySelectedObject);
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
}
void MenuControllerItem::Activate() {
	if (currentMenuItem) {
		currentMenuItem->Select();
		currentMenuItem->UnHighlight();
	}
}