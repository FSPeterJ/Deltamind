#include "Menu.h"
#include "MessageEvents.h"

void MenuOption::Select() {

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
	buttonPrefabMap[BUTTON_Resume] = 1;
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
	return (optionNumber < center ? center + distanceFromCenter : center - distanceFromCenter);
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
			buttons[1] = BUTTON_Restart;
			buttons[2] = BUTTON_Options;
			buttons[3] = BUTTON_Quit;
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
	for (int i = 0; i < buttons.size(); ++i) {
		MenuOption* newOption;
		float distFromCenter = FindDistanceFromCenter(i, (int)options.size(), 0.25f, 0.15f);
		//TODO: Need to spawn this in the correct place based off of the previous function
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(buttonPrefabMap[buttons[i]], { 0, 1, 0 }, (GameObject**)&newOption));
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