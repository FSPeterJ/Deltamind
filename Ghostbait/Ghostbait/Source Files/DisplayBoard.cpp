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
				screen->SwapComponentVarient<Material>("move_VR");
			else
				screen->SwapComponentVarient<Material>("move_NVR");
			break;
		case Type::Pause:
			if (isVR)
				screen->SwapComponentVarient<Material>("pause_VR");
			else
				screen->SwapComponentVarient<Material>("pause_NVR");
			break;
		case Type::Inventory:
			if (isVR)
				screen->SwapComponentVarient<Material>("inventory_VR");
			else
				screen->SwapComponentVarient<Material>("inventory_NVR");
			break;
		case Type::Items:
			if (isVR)
				screen->SwapComponentVarient<Material>("items_VR");
			else
				screen->SwapComponentVarient<Material>("items_NVR");
			break;
		case Type::Build:
			if (isVR)
				screen->SwapComponentVarient<Material>("build_VR");
			else
				screen->SwapComponentVarient<Material>("build_NVR");
			break;
		case Type::Shoot:
			if (isVR)
				screen->SwapComponentVarient<Material>("shoot_VR");
			else
				screen->SwapComponentVarient<Material>("shoot_NVR");
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