#include "MenuControllerItem.h"
#include "PhysicsExtension.h"
#include "Menu.h"


MenuControllerItem::MenuControllerItem() {

}

void MenuControllerItem::UpdateRay() {
	Raycast(DirectX::XMFLOAT3(position._41, position._42, position._43), DirectX::XMFLOAT3(position._31, position._32, position._33), &DirectX::XMFLOAT3(0, 0, 0), nullptr/*&currentlySelectedItem*/, 4);
}
void MenuControllerItem::Activate() {
	//MenuOption* option = dynamic_cast<MenuOption*>(currentlySelectedItem);
	//if (option) {
	//	option->Select();
	//}
}