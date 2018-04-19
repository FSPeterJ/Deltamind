#pragma once
#include "GameObject.h"
#include <map>

class Menu;
struct PhysicsComponent;

class MenuOption : public GameObject {
	//0.5 X 0.1 X 0.1
protected:
	Menu* menu;
	DirectX::XMFLOAT4X4 oldPos;
	DirectX::XMFLOAT3 oldColliderPoint;
	float popDistance = 0.15f;
	PhysicsComponent* pc = nullptr;
public:
	void Awake(Object* obj) override;
	inline void SetMenu(Menu* _menu) { menu = _menu; }
	inline void SetOldPos(DirectX::XMFLOAT4X4 mat) { oldPos = mat; }
	inline void SetOldColliderPoint(DirectX::XMFLOAT3 pos) { oldColliderPoint = pos; }
	inline PhysicsComponent* GetPhysics() const { return pc; }
	virtual void Select() = 0;
	void Highlight();
	void UnHighlight();
};

enum Template {
	MENU_Main,
	MENU_Pause,
	MENU_Options,
	MENU_Custom,
	MENU_Difficulty,
	MENU_QuitConfirm
};
enum Button {
	BUTTON_Resume,
	BUTTON_Restart,
	BUTTON_Options,
	BUTTON_Back,
	BUTTON_Exit,
	BUTTON_Quit,
	BUTTON_Play,
	BUTTON_Tutorial,
	BUTTON_Credits,
	BUTTON_Easy,
	BUTTON_Medium,
	BUTTON_Hard,

	BUTTON_MasterUp,
	BUTTON_MasterDown,
	BUTTON_MusicUp,
	BUTTON_MusicDown,
	BUTTON_SFXUp,
	BUTTON_SFXDown,
	BUTTON_BrightnessUp,
	BUTTON_BrightnessDown,
	BUTTON_MouseSensitivityUp,
	BUTTON_MouseSensitivityDown,
	BUTTON_AcceptOptions,
	BUTTON_CancelOptions,
	BUTTON_Revert,
	BUTTON_QuitConfirm,
	BUTTON_QuitCancel
};

class Menu {
public:
	enum ColumnType {
		OneColumn,
		TwoColumn,
	};
private:
	bool active = false;
	Template menu_template;
	std::map<Button, unsigned> buttonPrefabMap;
	std::vector<Button> buttons;
	std::vector<MenuOption*> options;
	Transform* camera = nullptr;
	DirectX::XMFLOAT4X4 spawnPos = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	Menu* parent = nullptr;
	Menu* child = nullptr;

	ColumnType columnType = OneColumn;

	DirectX::XMFLOAT4X4 FindCenter(float distFromPlayer = 1);
	float FindDistanceFromCenter(int optionNumber, int optionCount, float optionHeight, float gapHeight);
	void AssignPrefabIDs();

public:

	Menu();
	Menu(Template t, std::vector<Button> buttons = std::vector<Button>(), ColumnType _columnType = OneColumn);
	void Create(Template t, std::vector<Button> buttons = std::vector<Button>(), ColumnType _columnType = OneColumn);
	void SetCamera(Transform* _camera);
	inline void SetParent(Menu* _parent) { parent = _parent; };
	inline void SetChild(Menu* _child) { child = _child; };
	inline void SetSpawnPos(DirectX::XMFLOAT4X4& pos) { spawnPos = pos; }
	void CreateAndLoadChild(Template t, std::vector<Button> buttons = std::vector<Button>(), ColumnType _columnType = OneColumn);
	void LoadParent();
	void Show(bool useCamera = true);
	void Hide();
	~Menu();
};


class PlayButton : public MenuOption {
	void Select() override;
};
class TutorialButton : public MenuOption {
	void Select() override;
};
class OptionsButton : public MenuOption {
	void Select() override;
};
class CreditsButton : public MenuOption {
	void Select() override;
};
class QuitButton : public MenuOption {
	void Select() override;
};
class ResumeButton : public MenuOption {
	void Select() override;
};
class RestartButton : public MenuOption {
	void Select() override;
};
class ExitButton : public MenuOption {
	void Select() override;
};
class BackButton : public MenuOption {
	void Select() override;
};
class EasyButton : public MenuOption {
	void Select() override;
};
class MediumButton : public MenuOption {
	void Select() override;
};
class HardButton : public MenuOption {
	void Select() override;
};

class MasterUpButton : public MenuOption {
	void Select() override;
};
class MasterDownButton : public MenuOption {
	void Select() override;
};
class MusicUpButton : public MenuOption {
	void Select() override;
};
class MusicDownButton : public MenuOption {
	void Select() override;
};
class SFXUpButton : public MenuOption {
	void Select() override;
};
class SFXDownButton : public MenuOption {
	void Select() override;
};
class BrightnessUpButton : public MenuOption {
	void Select() override;
};
class BrightnessDownButton : public MenuOption {
	void Select() override;
};
class MouseSensitivityUpButton : public MenuOption {
	void Select() override;
};
class MouseSensitivityDownButton : public MenuOption {
	void Select() override;
};
class AcceptOptionsButton : public MenuOption {
	void Select() override;
};
class CancelOptionsButton : public MenuOption {
	void Select() override;
};

class RevertOptionsButton : public MenuOption {
	void Select() override;
};

class QuitConfirmButton : public MenuOption {
	void Select() override;
};

class QuitCancelButton : public MenuOption {
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

