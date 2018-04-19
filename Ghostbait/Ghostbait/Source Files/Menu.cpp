#include "Menu.h"
#include "MessageEvents.h"
#include "Console.h"
#include "ObjectFactory.h"
#include "PhysicsComponent.h"
#include "OptionsManager.h"

void MenuOption::Awake(Object* obj) {
	GameObject::Awake(obj);
	pc = GetComponent<PhysicsComponent>();
}
void MenuOption::Select() {
	//Console::WriteLine << "Menu Option: " << this << " was selected!";
}
void MenuOption::UnHighlight() {
	//Console::WriteLine << "Menu Option: " << this << " was Un-highlighted!";
	transform.SetMatrix(oldPos);
	if (pc && pc->colliders.size()) {
		pc->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner = oldColliderPoint;
	}

}
void MenuOption::Highlight() {
	//Console::WriteLine << "Menu Option: " << this << " was highlighted!";
	DirectX::XMFLOAT3 newPos = { oldPos._41, oldPos._42, oldPos._43 };
	newPos.x -= oldPos._31 * popDistance;
	newPos.y -= oldPos._32 * popDistance;
	newPos.z -= oldPos._33 * popDistance;
	transform.SetPosition(newPos);
	if (pc && pc->colliders.size()) {
		pc->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner.z += popDistance * 2;
	}
}

Menu::Menu() {
	AssignPrefabIDs();
}
Menu::Menu(Template t, std::vector<Button> buttons, ColumnType _columnType) {
	AssignPrefabIDs();
	Create(t, buttons, _columnType);
}
void Menu::SetCamera(Transform* _camera) {
	camera = _camera;
}
void Menu::AssignPrefabIDs() {
	buttonPrefabMap[BUTTON_Play] = ObjectFactory::CreatePrefab(&std::string("Assets/PlayButton.ghost"));
	buttonPrefabMap[BUTTON_Tutorial] = ObjectFactory::CreatePrefab(&std::string("Assets/TutorialButton.ghost"));
	buttonPrefabMap[BUTTON_Options] = ObjectFactory::CreatePrefab(&std::string("Assets/OptionsButton.ghost"));
	buttonPrefabMap[BUTTON_Credits] = ObjectFactory::CreatePrefab(&std::string("Assets/CreditsButton.ghost"));
	buttonPrefabMap[BUTTON_Quit] = ObjectFactory::CreatePrefab(&std::string("Assets/QuitButton.ghost"));

	buttonPrefabMap[BUTTON_Resume] = ObjectFactory::CreatePrefab(&std::string("Assets/ResumeButton.ghost"));
	buttonPrefabMap[BUTTON_Restart] = ObjectFactory::CreatePrefab(&std::string("Assets/RestartButton.ghost"));
	buttonPrefabMap[BUTTON_Exit] = ObjectFactory::CreatePrefab(&std::string("Assets/ExitButton.ghost"));


	buttonPrefabMap[BUTTON_Easy] = ObjectFactory::CreatePrefab(&std::string("Assets/EasyButton.ghost"));
	buttonPrefabMap[BUTTON_Medium] = ObjectFactory::CreatePrefab(&std::string("Assets/MediumButton.ghost"));
	buttonPrefabMap[BUTTON_Hard] = ObjectFactory::CreatePrefab(&std::string("Assets/HardButton.ghost"));

	buttonPrefabMap[BUTTON_Back] = ObjectFactory::CreatePrefab(&std::string("Assets/BackButton.ghost"));

	buttonPrefabMap[BUTTON_MasterUp] = ObjectFactory::CreatePrefab(&std::string("Assets/MasterUpButton.ghost"));
	buttonPrefabMap[BUTTON_MasterDown] = ObjectFactory::CreatePrefab(&std::string("Assets/MasterDownButton.ghost"));
	buttonPrefabMap[BUTTON_MusicUp] = ObjectFactory::CreatePrefab(&std::string("Assets/MusicUpButton.ghost"));
	buttonPrefabMap[BUTTON_MusicDown] = ObjectFactory::CreatePrefab(&std::string("Assets/MusicDownButton.ghost"));
	buttonPrefabMap[BUTTON_SFXUp] = ObjectFactory::CreatePrefab(&std::string("Assets/SFXUpButton.ghost"));
	buttonPrefabMap[BUTTON_SFXDown] = ObjectFactory::CreatePrefab(&std::string("Assets/SFXDownButton.ghost"));
	buttonPrefabMap[BUTTON_BrightnessUp] = ObjectFactory::CreatePrefab(&std::string("Assets/BrightnessUpButton.ghost"));
	buttonPrefabMap[BUTTON_BrightnessDown] = ObjectFactory::CreatePrefab(&std::string("Assets/BrightnessDownButton.ghost"));
	buttonPrefabMap[BUTTON_MouseSensitivityUp] = ObjectFactory::CreatePrefab(&std::string("Assets/MouseSensitivityUpButton.ghost"));
	buttonPrefabMap[BUTTON_MouseSensitivityDown] = ObjectFactory::CreatePrefab(&std::string("Assets/MouseSensitivityDownButton.ghost"));
	buttonPrefabMap[BUTTON_AcceptOptions] = ObjectFactory::CreatePrefab(&std::string("Assets/AcceptOptionsButton.ghost"));
	buttonPrefabMap[BUTTON_CancelOptions] = ObjectFactory::CreatePrefab(&std::string("Assets/CancelOptionsButton.ghost"));

	buttonPrefabMap[BUTTON_Next] = ObjectFactory::CreatePrefab(&std::string("Assets/NextButton.ghost"));
	buttonPrefabMap[BUTTON_Skip] = ObjectFactory::CreatePrefab(&std::string("Assets/SkipButton.ghost"));

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
void Menu::Create(Template t, std::vector<Button> _buttons, ColumnType _columnType) {
	menu_template = t;
	switch (t) {
		case MENU_Main:
			buttons.empty();
			buttons.resize(5);
			buttons[0] = BUTTON_Play;
			buttons[1] = BUTTON_Tutorial;
			buttons[2] = BUTTON_Options;
			buttons[3] = BUTTON_Credits;
			buttons[4] = BUTTON_Quit;
			columnType = OneColumn;
			break;
		case MENU_Pause:
			buttons.empty();
			buttons.resize(3);
			buttons[0] = BUTTON_Resume;
			buttons[1] = BUTTON_Restart;
			buttons[2] = BUTTON_Exit;
			columnType = OneColumn;
			MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->Show(); });
			MessageEvents::Subscribe(EVENT_GameUnPause, [=](EventMessageBase* e) {this->Hide(); });
			break;
		case MENU_Options:
			buttons.empty();
			buttons.resize(12);
			buttons[0] = BUTTON_MasterUp;
			buttons[1] = BUTTON_MasterDown;
			buttons[2] = BUTTON_MusicUp;
			buttons[3] = BUTTON_MusicDown;
			buttons[4] = BUTTON_SFXUp;
			buttons[5] = BUTTON_SFXDown;
			buttons[6] = BUTTON_BrightnessUp;
			buttons[7] = BUTTON_BrightnessDown;
			buttons[8] = BUTTON_MouseSensitivityUp;
			buttons[9] = BUTTON_MouseSensitivityDown;
			buttons[10] = BUTTON_AcceptOptions;
			buttons[11] = BUTTON_CancelOptions;
			columnType = TwoColumn;
			break;
		case MENU_Difficulty:
			buttons.empty();
			buttons.resize(4);
			buttons[0] = BUTTON_Back;
			buttons[1] = BUTTON_Easy;
			buttons[2] = BUTTON_Medium;
			buttons[3] = BUTTON_Hard;
			columnType = OneColumn;
			break;
		case MENU_SplashScreen:
			buttons.empty();
			buttons.resize(2);
			buttons[0] = BUTTON_Next;
			buttons[1] = BUTTON_Skip;
			columnType = TwoColumn;
			break;
		case MENU_Custom:
			buttons.empty();
			buttons = _buttons;
			columnType = _columnType;
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
	switch (columnType) {
		case OneColumn:
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
				options[i]->SetOldColliderPoint(options[i]->GetPhysics()->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner);
				options[i]->UnHighlight();
				newOption->UnRender();
				newOption->RenderToFront();
			}
			break;
		case TwoColumn:
			for (size_t i = 0; i < buttons.size(); ++i) {
				MenuOption* newOption;
				DirectX::XMFLOAT4X4 newObjPos;
				int buttonID = 0;
				float shift = 0.3f;
				if (i % 2 == 0) {
					buttonID = (int)i / 2;
					shift = -shift;
				}
				else {
					buttonID = (int)((float)i / 2.0f);
				}
				float distFromCenter = FindDistanceFromCenter((int)buttonID, (int)(options.size() / 2), 0.25f, 0.05f);
				DirectX::XMStoreFloat4x4(&newObjPos, center_M * DirectX::XMMatrixTranslation(shift, distFromCenter, 0));
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuOption>(buttonPrefabMap[buttons[i]], newObjPos, &newOption));
				newOption->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
				newOption->SetMenu(this);
				newOption->Enable();
				newOption->PersistOnReset();
				options[i] = newOption;
				options[i]->SetOldPos(options[i]->transform.GetMatrix());
				options[i]->SetOldColliderPoint(options[i]->GetPhysics()->colliders[0].colliderData->colliderInfo.boxCollider.topRightFrontCorner);
				options[i]->UnHighlight();
				newOption->UnRender();
				newOption->RenderToFront();
			}
			break;
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
void Menu::CreateAndLoadChild(Template t, std::vector<Button> buttons, ColumnType _columnType) {
	child = new Menu(t, buttons, _columnType);
	child->SetCamera(camera);
	DirectX::XMFLOAT4X4 newSpawn = spawnPos;
	if (t == Template::MENU_Options) {
		//newSpawn._41 += 0.25f;
	}
	child->SetSpawnPos(newSpawn);
	child->SetParent(this);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {child->Show(); });
}
void Menu::LoadParent() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] { parent->Show();});
}
Menu::~Menu() {
	if (child) delete child;
}

void PlayButton::Select() {
	MenuOption::Select();
	menu->Hide();
	menu->CreateAndLoadChild(MENU_Difficulty);
}
void TutorialButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_ChangeScene, ChangeSceneMessage("Tutorial"));
}
void OptionsButton::Select() {
	MenuOption::Select();
	menu->Hide();
	OptionsManager::GetInstance().CacheValues();
	menu->CreateAndLoadChild(MENU_Options);
}
void CreditsButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_ChangeScene, ChangeSceneMessage("Credits"));
}
void QuitButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	MessageEvents::SendQueueMessage(EVENT_Late, []() { MessageEvents::SendMessage(EVENT_GameQuit, EventMessageBase()); });
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
void ExitButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	MessageEvents::SendQueueMessage(EVENT_Late, []() { MessageEvents::SendMessage(EVENT_GameExit, EventMessageBase()); });
}
void BackButton::Select() {
	MenuOption::Select();
	menu->Hide();
	menu->LoadParent();
}
void EasyButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_Start, StartEventMessage("Level Files//level0_easy.xml"));
}
void MediumButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_Start, StartEventMessage("Level Files//level0.xml"));
}
void HardButton::Select() {
	MenuOption::Select();
	menu->Hide();
	MessageEvents::SendMessage(EVENT_Start, StartEventMessage("Level Files//level0_hard.xml"));
}

void MasterUpButton::Select() {
	float tempVolume = OptionsManager::GetInstance().GetMasterVolume();
	if (tempVolume < 1.0f)
		OptionsManager::GetInstance().SetMasterVolume(tempVolume + 0.1f);
	MenuOption::Select();
}
void MasterDownButton::Select() {
	float tempVolume = OptionsManager::GetInstance().GetMasterVolume();
	if (tempVolume > 0.0f)
		OptionsManager::GetInstance().SetMasterVolume(tempVolume - 0.1f);
	MenuOption::Select();
}
void MusicUpButton::Select() {
	MenuOption::Select();
}
void MusicDownButton::Select() {
	MenuOption::Select();
}
void SFXUpButton::Select() {
	float tempVolume = OptionsManager::GetInstance().GetSFXVolume();
	if (tempVolume < 100.0f)
		OptionsManager::GetInstance().SetSFXVolume(tempVolume + 10.0f);
	MenuOption::Select();
}
void SFXDownButton::Select() {
	float tempVolume = OptionsManager::GetInstance().GetSFXVolume();
	if (tempVolume > 0.0f)
		OptionsManager::GetInstance().SetSFXVolume(tempVolume - 10.0f);
	MenuOption::Select();
}
void BrightnessUpButton::Select() {
	float tempGam = OptionsManager::GetInstance().GetGamma();
	if (tempGam < 0.5f)
		OptionsManager::GetInstance().SetGamma(tempGam + 0.05f);
	MenuOption::Select();
}
void BrightnessDownButton::Select() {
	float tempGam = OptionsManager::GetInstance().GetGamma();
	if(tempGam > 0.0f)
		OptionsManager::GetInstance().SetGamma(tempGam - 0.05f);
	MenuOption::Select();
}
void MouseSensitivityUpButton::Select() {
	float currSens = OptionsManager::GetInstance().GetSensitivity();
	if (currSens < 0.015f)
		OptionsManager::GetInstance().SetSensitivity(currSens + 0.00135f);
	MenuOption::Select();
}
void MouseSensitivityDownButton::Select() {
	float currSens = OptionsManager::GetInstance().GetSensitivity();
	if (currSens > 0.0015f)
		OptionsManager::GetInstance().SetSensitivity(currSens - 0.00135f);
	MenuOption::Select();
}
void AcceptOptionsButton::Select() {
	menu->Hide();
	menu->LoadParent();
	MenuOption::Select();
}
void CancelOptionsButton::Select() {
	OptionsManager::GetInstance().Cancel();
	menu->Hide();
	menu->LoadParent();
	MenuOption::Select();
}

void NextButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_NextLogo, EventMessageBase());
}
void SkipButton::Select() {
	MenuOption::Select();
	MessageEvents::SendMessage(EVENT_Start, StartEventMessage(""));
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
