#pragma once
#include "GameObject.h"
#include <map>

class MenuOption : public GameObject {
public:
	void Select();
};

enum Template {
	MENU_Main,
	MENU_Pause,
	MENU_Custom
};
enum Button {
	BUTTON_Resume,
	BUTTON_Restart,
	BUTTON_Options,
	BUTTON_Quit,

	BUTTON_StartGame,
	BUTTON_SelectLevel,
};

class Menu {
	bool active = false;
	Template menu_template;
	std::map<Button, int> buttonPrefabMap;
	std::vector<Button> buttons;
	std::vector<MenuOption*> options;

	float FindDistanceFromCenter(int optionNumber, int optionCount, float optionHeight, float gapHeight);
	void AssignPrefabIDs();

	void GamePauseEvent();
public:
	Menu();
	Menu(Template t, std::vector<Button> buttons = std::vector<Button>());
	void Create(Template t, std::vector<Button> buttons = std::vector<Button>());
	void Show();
	void Hide();
	void Cleanup();
};


class ResumeCube : public MenuOption {

};

class RestartCube : public MenuOption {

};

class OptionsCube : public MenuOption {

};

class QuitCube : public MenuOption {

};

class StartGameCube : public MenuOption {

};
class SelectLevelCube : public MenuOption {

};