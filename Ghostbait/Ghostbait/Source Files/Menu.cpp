#include "Menu.h"
#include "MessageEvents.h"
#include "VRManager.h"

void MenuOption::Select() {

}
void MenuOption::UnHighlight() {

}
void MenuOption::Highlight() {

}

Menu::Menu() {
	AssignPrefabIDs();
	Create(MENU_Pause);
}
Menu::Menu(Template t, std::vector<Button> buttons) {
	AssignPrefabIDs();
	Create(t, buttons);
}
void Menu::AssignPrefabIDs() {
	buttonPrefabMap[BUTTON_Resume] = 20;
	buttonPrefabMap[BUTTON_Restart] = 1;
	buttonPrefabMap[BUTTON_Options] = 1;
	buttonPrefabMap[BUTTON_Quit] = 1;
	buttonPrefabMap[BUTTON_StartGame] = 1;
	buttonPrefabMap[BUTTON_SelectLevel] = 1;
}
void Menu::GamePauseEvent() {
	if (active)
		Hide();
	else
		Show();
}

DirectX::XMFLOAT4X4 Menu::FindCenter(float distFromPlayer) {
	DirectX::XMMATRIX center_M;
	DirectX::XMMATRIX player_M = DirectX::XMLoadFloat4x4(&VRManager::GetInstance().GetPlayerPosition());
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(player_M.r[0], DirectX::XMVectorSet(0, 1, 0, 0)));
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(forward, distFromPlayer));
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

	//if odd number, find middle option
	//if (optionCount % 2 == 1) {
	//	float middleOption = ceil(optionCount * 0.5f);
	//	if (optionNumber == middleOption) return 0;
	//	
	//	int countBetweenOptionAndCenter = middleOption - optionNumber - 1;
	//	float distanceFromCenter = (optionHeight + gapHeight + ((optionHeight + gapHeight) * (float)countBetweenOptionAndCenter));
	//	return (optionNumber < middleOption ? center + distanceFromCenter : center - distanceFromCenter);
	//}

}
void Menu::Create(Template t, std::vector<Button> _buttons) {
	switch (t) {
		case MENU_Main:
			buttons.empty();
			buttons.resize(4);
			buttons[0] = BUTTON_StartGame;
			buttons[1] = BUTTON_SelectLevel;
			buttons[2] = BUTTON_Options;
			buttons[3] = BUTTON_Quit;
			break;
		case MENU_Pause:
			buttons.empty();
			buttons.resize(4);
			buttons[0] = BUTTON_Resume;
			buttons[1] = BUTTON_Resume;
			buttons[2] = BUTTON_Resume;
			buttons[3] = BUTTON_Resume;
			//buttons[1] = BUTTON_Restart;
			//buttons[2] = BUTTON_Options;
			//buttons[3] = BUTTON_Quit;
			MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->GamePauseEvent(); });
			break;
		case MENU_Custom:
			buttons.empty();
			buttons = _buttons;
			break;
	}
}
void Menu::Show() {
	active = true;
	options.resize(buttons.size());
	DirectX::XMFLOAT4X4 center = FindCenter();
	DirectX::XMMATRIX center_M = DirectX::XMLoadFloat4x4(&center);
	for (int i = 0; i < buttons.size(); ++i) {
		MenuOption* newOption;
		float distFromCenter = FindDistanceFromCenter(i, (int)options.size(), 0.25f, 0.05f);
		//TODO: Need to spawn this in the correct place based off of the previous function
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(buttonPrefabMap[buttons[i]], {0, 0, 0}, (GameObject**)&newOption));
		DirectX::XMStoreFloat4x4(&newOption->position, center_M * DirectX::XMMatrixTranslation(0, distFromCenter, 0));
		newOption->SetMenu(this);
		options[i] = newOption;
	}
}
void Menu::Hide() {
	active = false;
	for (int i = 0; i < options.size(); ++i) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {options[i]->Destroy(); });
	}
	options.empty();
}

void ResumeButton::Select() {
	MenuOption::Select();
	menu->Hide();
}
void RestartButton::Select() {
	MenuOption::Select();
	MenuOption::Select();
	menu->Hide();
}
void QuitButton::Select() {
	MenuOption::Select();

}

void OptionsButton::Select() {
	MenuOption::Select();

}
void StartGameButton::Select() {
	MenuOption::Select();

}
void SelectLevelButton::Select() {
	MenuOption::Select();

}