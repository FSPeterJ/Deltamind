#include "DisplayBoard.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "Material.h"
#include "VRManager.h"

void DisplayBoard::SetType(Type _type) {
	switch (_type) {
		case Type::Default:
			break;
		case Type::Move:
			if (isVR)
				screen->SwapComponentVariant<Material>("move_VR");
			else
				screen->SwapComponentVariant<Material>("move_NVR");
			break;
		case Type::Pause:
			if (isVR)
				screen->SwapComponentVariant<Material>("pause_VR");
			else
				screen->SwapComponentVariant<Material>("pause_NVR");
			break;
		case Type::Inventory:
			if (isVR)
				screen->SwapComponentVariant<Material>("inventory_VR");
			else
				screen->SwapComponentVariant<Material>("inventory_NVR");
			break;
		case Type::Items:
			if (isVR)
				screen->SwapComponentVariant<Material>("items_VR");
			else
				screen->SwapComponentVariant<Material>("items_NVR");
			break;
		case Type::Build:
			if (isVR)
				screen->SwapComponentVariant<Material>("build_VR");
			else
				screen->SwapComponentVariant<Material>("build_NVR");
			break;
		case Type::Shoot:
			if (isVR)
				screen->SwapComponentVariant<Material>("shoot_VR");
			else
				screen->SwapComponentVariant<Material>("shoot_NVR");
			break;
	}
}

void DisplayBoard::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Display_Lifter.ghost")), { 0, 0, 0 }, &lifter));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/DisplayBoard_Screen.ghost")), { 0, 0, 0 }, &screen));
	isVR = VRManager::GetInstance().IsEnabled();
}
void DisplayBoard::Update() {
	GameObject::Update();
	if (lifter) {
		lifter->transform.SetMatrix(transform.GetMatrix());
		screen->transform.SetMatrix(transform.GetMatrix());
	}
}
void DisplayBoard::Destroy() {
	if (lifter) lifter->Destroy(); 
	if (screen) screen->Destroy(); 
	GameObject::Destroy();
}