#pragma once
#include "GameObject.h"
#include <map>

class Menu;

class MenuOption : public GameObject {
	//0.5 X 0.1 X 0.1
protected:
	Menu* menu;
public:
	inline void SetMenu(Menu* _menu) { menu = _menu; };
	virtual void Select() = 0;
	void Highlight();
	void UnHighlight();
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
	std::map<Button, char*> buttonPrefabMap;
	std::vector<Button> buttons;
	std::vector<MenuOption*> options;
	DirectX::XMFLOAT4X4 FindCenter(float distFromPlayer = 1);
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


class ResumeButton : public MenuOption {
	void Select() override;
};

class RestartButton : public MenuOption {
	void Select() override;
};

class OptionsButton : public MenuOption {
	void Select() override;
};

class QuitButton : public MenuOption {
	void Select() override;
};

class StartGameButton : public MenuOption {
	void Select() override;
};
class SelectLevelButton : public MenuOption {
	void Select() override;
};