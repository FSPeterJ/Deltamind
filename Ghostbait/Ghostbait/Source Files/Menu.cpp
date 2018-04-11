#include "Menu.h"
#include "MessageEvents.h"
#include "Console.h"
#include "ObjectFactory.h"
#include "PhysicsComponent.h"

void MenuOption::Select() {
	//Console::WriteLine << "Menu Option: " << this << " was selected!";
}
void MenuOption::UnHighlight() {
	//Console::WriteLine << "Menu Option: " << this << " was Un-highlighted!";
	transform.SetMatrix(oldPos);
	PhysicsComponent* comp = GetComponent<PhysicsComponent>();
	if (comp && comp->colliders.size()) {
		comp->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner = oldColliderPoint;
	}

}
void MenuOption::Highlight() {
	//Console::WriteLine << "Menu Option: " << this << " was highlighted!";
	DirectX::XMFLOAT3 newPos = { oldPos._41, oldPos._42, oldPos._43 };
	newPos.x -= oldPos._31 * popDistance;
	newPos.y -= oldPos._32 * popDistance;
	newPos.z -= oldPos._33 * popDistance;
	transform.SetPosition(newPos);
	PhysicsComponent* comp = GetComponent<PhysicsComponent>();
	if (comp && comp->colliders.size()) {
		comp->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner.z += popDistance * 2;
	}
}

Menu::Menu() {
	AssignPrefabIDs();
}
Menu::Menu(Template t, std::vector<Button> buttons) {
	AssignPrefabIDs();
	Create(t, buttons);
}
void Menu::SetCamera(Transform* _camera) {
	camera = _camera;
}
void Menu::AssignPrefabIDs() {
	buttonPrefabMap[BUTTON_Resume] = ObjectFactory::CreatePrefab(&std::string("Assets/ResumeButton.ghost"));
	buttonPrefabMap[BUTTON_Restart] = ObjectFactory::CreatePrefab(&std::string("Assets/RestartButton.ghost"));
	buttonPrefabMap[BUTTON_Quit] = ObjectFactory::CreatePrefab(&std::string("Assets/QuitButton.ghost"));
	buttonPrefabMap[BUTTON_Play] = ObjectFactory::CreatePrefab(&std::string("Assets/PlayButton.ghost"));
	buttonPrefabMap[BUTTON_Exit] = ObjectFactory::CreatePrefab(&std::string("Assets/ExitButton.ghost"));
	buttonPrefabMap[BUTTON_Options] = ObjectFactory::CreatePrefab(&std::string("Assets/OptionsButton.ghost"));
	buttonPrefabMap[BUTTON_Back] = ObjectFactory::CreatePrefab(&std::string("Assets/BackButton.ghost"));
	buttonPrefabMap[BUTTON_Tutorial] = ObjectFactory::CreatePrefab(&std::string("Assets/TutorialButton.ghost"));
}

DirectX::XMFLOAT4X4 Menu::FindCenter(float distFromPlayer) {
	DirectX::XMMATRIX center_M;
	DirectX::XMMATRIX player_M = DirectX::XMLoadFloat4x4(&camera->GetMatrix());
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(player_M.r[0], DirectX::XMVectorSet(0, 1, 0, 0)));
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(DirectX::XMVectorScale(forward, 1.5f), distFromPlayer));
	center_M = player_M * translationMat;
	center_M.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
	center_M.r[2] = forward;
	center_M.r[0] = DirectX::XMVector3Cross(center_M.r[1], center_M.r[2]);

	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, center_M);
	return result;
}
float Menu::FindDistanceFromCenter(int optionNumber, int optionCount, float optionHeight, float gapHeight) {
	optionNumber += 1;
	float center = (optionCount * 0.5f) + 0.5f;
	int countBetweenOptionAndCenter = (int)(abs((float)(center - (float)optionNumber)));
	float distanceFromCenter = 0;
	
	//Even Number
	if (optionCount % 2 == 0) {
		distanceFromCenter = (gapHeight * 0.5f) + (optionHeight * 0.5f) + ((gapHeight + optionHeight) * (float)countBetweenOptionAndCenter);
	}
	//Odd Number
	else {
		if (center == optionNumber) return 0;
		countBetweenOptionAndCenter -= 1;
		distanceFromCenter = (optionHeight + gapHeight + ((optionHeight + gapHeight) * (float)countBetweenOptionAndCenter));
	}
	return (optionNumber < center ? distanceFromCenter : -distanceFromCenter);
}
void Menu::Create(Template t, std::vector<Button> _buttons) {
	menu_template = t;
	switch (t) {
		case MENU_Main:
			buttons.empty();
			buttons.resize(4);
			buttons[0] = BUTTON_Play;
			buttons[1] = BUTTON_Tutorial;
			buttons[2] = BUTTON_Options;
			buttons[3] = BUTTON_Quit;
			break;
		case MENU_Pause:
			buttons.empty();
			buttons.resize(3);
			buttons[0] = BUTTON_Resume;
			buttons[1] = BUTTON_Restart;
			buttons[2] = BUTTON_Exit;
			MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->Show(); });
			MessageEvents::Subscribe(EVENT_GameUnPause, [=](EventMessageBase* e) {this->Hide(); });
			break;
		case MENU_Options:
			buttons.empty();
			buttons.resize(1);
			buttons[0] = BUTTON_Back;
			break;
		case MENU_Custom:
			buttons.empty();
			buttons = _buttons;
			break;
	}
}
void Menu::Show(bool useCamera) {
	if (active) return;
	active = true;
	if (child) {
		delete child;
		child = nullptr;
	}

	options.resize(buttons.size());
	DirectX::XMFLOAT4X4 center = (useCamera && camera) ? FindCenter() : spawnPos;
	DirectX::XMMATRIX center_M = DirectX::XMLoadFloat4x4(&center);
	for (size_t i = 0; i < buttons.size(); ++i) {
		MenuOption* newOption;
		DirectX::XMFLOAT4X4 newObjPos;
		float distFromCenter = FindDistanceFromCenter((int)i, (int)options.size(), 0.25f, 0.05f);
		DirectX::XMStoreFloat4x4(&newObjPos, center_M * DirectX::XMMatrixTranslation(0, distFromCenter, 0));
		MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuOption>(buttonPrefabMap[buttons[i]], newObjPos, &newOption));
		newOption->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
		newOption->SetMenu(this);
		newOption->Enable();
		newOption->PersistOnReset();
		options[i] = newOption;
		options[i]->SetOldPos(options[i]->transform.GetMatrix());
		options[i]->SetOldColliderPoint(options[i]->GetComponent<PhysicsComponent>()->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner);
		options[i]->UnHighlight();
		newOption->UnRender();
		newOption->RenderToFront();
	}
}
void Menu::Hide() {
	if (!active) return;
	active = false;
	for (size_t i = 0; i < options.size(); ++i) {
		options[i]->UnHighlight();
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {options[i]->Destroy(); });
	}
	options.empty();
}
void Menu::CreateAndLoadChild(Template t, std::vector<Button> buttons) {
	child = new Menu(t, buttons);
	child->SetCamera(camera);
	child->SetSpawnPos(spawnPos);
	child->SetParent(this);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {child->Show(); });
}
void Menu::LoadParent() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] { parent->Show();});
}
Menu::~Menu() {
	if (child) delete child;
}

void ResumeButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
}
void RestartButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	MessageEvents::SendMessage(EVENT_GameRestart, EventMessageBase());
}
void QuitButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	MessageEvents::SendQueueMessage(EVENT_Late, []() { MessageEvents::SendMessage(EVENT_GameQuit, EventMessageBase()); });
}
void ExitButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	MessageEvents::SendQueueMessage(EVENT_Late, []() { MessageEvents::SendMessage(EVENT_GameExit, EventMessageBase()); });
}
void OptionsButton::Select() {
	menu->Hide();
	menu->CreateAndLoadChild(MENU_Options);
	MenuOption::Select();
}
void BackButton::Select() {
	menu->Hide();
	menu->LoadParent();
	MenuOption::Select();
}
void PlayButton::Select() {
	menu->Hide();
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_Start, EventMessageBase());
}
void TutorialButton::Select() {
	menu->Hide();
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_TutorialHit, EventMessageBase());
}


//Other
void MenuCube::Update() {
	//position.m[3][1] += 0.4f * (float) GhostTime::DeltaTime();
	//if(position.m[3][1] > 1.5f) {
	//	Disable();
	//}
	GameObject::Update();
}

void MenuCube::OnCollision(GameObject* other) {
	if(other->GetTag() == "Bullet") {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		Console::WriteLine << "StartCube Shot!";
		MessageEvents::SendMessage(EVENT_Start, EventMessageBase());
		//GameObject* obj;
		//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(8/*Core*/, {0, 1.5f, 0}, &obj));
		//DirectX::XMStoreFloat4x4(&obj->position,
		//	DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	}
	GameObject::OnCollision(other);
}
