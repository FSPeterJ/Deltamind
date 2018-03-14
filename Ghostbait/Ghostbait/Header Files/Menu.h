#pragma once
#include "GameObject.h"
#include <map>

class Menu;

class MenuOption : public GameObject {
	//0.5 X 0.1 X 0.1
protected:
	Menu* menu;
	DirectX::XMFLOAT4X4 oldPos;
	DirectX::XMFLOAT3 oldColliderPoint;
	float popDistance = 0.15f;
public:
	inline void SetMenu(Menu* _menu) { menu = _menu; }
	inline void SetOldPos(DirectX::XMFLOAT4X4 mat) { oldPos = mat; }
	inline void SetOldColliderPoint(DirectX::XMFLOAT3 pos) { oldColliderPoint = pos; }
	virtual void Select() = 0;
	void Highlight();
	void UnHighlight();
};

enum Template {
	MENU_Main,
	MENU_Pause,
	MENU_Options,
	MENU_Custom
};
enum Button {
	BUTTON_Resume,
	BUTTON_Restart,
	BUTTON_Options,
	BUTTON_Back,
	BUTTON_Exit,
	BUTTON_Quit,

	BUTTON_Play,
	BUTTON_ChangeLevel,
};

class Menu {
	bool active = false;
	Template menu_template;
	std::map<Button, unsigned> buttonPrefabMap;
	std::vector<Button> buttons;
	std::vector<MenuOption*> options;
	Transform* camera = nullptr;
	DirectX::XMFLOAT4X4 spawnPos = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	Menu* parent = nullptr;
	Menu* child = nullptr;

	DirectX::XMFLOAT4X4 FindCenter(float distFromPlayer = 1);
	float FindDistanceFromCenter(int optionNumber, int optionCount, float optionHeight, float gapHeight);
	void AssignPrefabIDs();

public:
	Menu();
	Menu(Template t, std::vector<Button> buttons = std::vector<Button>());
	void Create(Template t, std::vector<Button> buttons = std::vector<Button>());
	void SetCamera(Transform* _camera);
	inline void SetParent(Menu* _parent) { parent = _parent; };
	inline void SetChild(Menu* _child) { child = _child; };
	inline void SetSpawnPos(DirectX::XMFLOAT4X4& pos) { spawnPos = pos; }
	void CreateAndLoadChild(Template t, std::vector<Button> buttons = std::vector<Button>());
	void LoadParent();
	void Show(bool useCamera = true);
	void Hide();
	~Menu();
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

class ExitButton : public MenuOption {
	void Select() override;
};
class BackButton : public MenuOption {
	void Select() override;
};
class PlayButton : public MenuOption {
	void Select() override;
};
class ChangeLevelButton : public MenuOption {
	void Select() override;
};







//Other
class MenuCube: public GameObject {
	void Update();
public:
	void OnCollision(GameObject* other);

	void Enable() override {
		GameObject::Enable();
	}

	void Destroy() {
		GameObject::Destroy();
	};
};
class CoreCube: public GameObject {
	bool enemyTouched = false;
public:
	CoreCube() { SetTag("Core"); };
	void OnCollision(GameObject* other);
	void Destroy() override;
};
