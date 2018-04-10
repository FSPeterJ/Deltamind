#include "DisplayBoard.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "Material.h"
#include "VRManager.h"

void DisplayBoard::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Display_Lifter.ghost")), { 0, 0, 0 }, &lifter));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/DisplayBoard_Screen.ghost")), { 0, 0, 0 }, &screen));
}

void DisplayBoard::SetType(Type _type) {
	switch (_type) {
		case Type::Default:
			break;
		case Type::Move:
			if (VRManager::GetInstance().IsEnabled())
				screen->SwapComponentVarient<Material>("move_VR");
			else
				screen->SwapComponentVarient<Material>("move_NVR");
			break;
		case Type::Pause:
			if (VRManager::GetInstance().IsEnabled())
				screen->SwapComponentVarient<Material>("pause_VR");
			else
				screen->SwapComponentVarient<Material>("pause_NVR");
			break;
		case Type::Inventory:
			if (VRManager::GetInstance().IsEnabled())
				screen->SwapComponentVarient<Material>("inventory_VR");
			else
				screen->SwapComponentVarient<Material>("inventory_NVR");
			break;
		case Type::Items:
			if (VRManager::GetInstance().IsEnabled())
				screen->SwapComponentVarient<Material>("items_VR");
			else
				screen->SwapComponentVarient<Material>("items_NVR");
			break;
	}
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