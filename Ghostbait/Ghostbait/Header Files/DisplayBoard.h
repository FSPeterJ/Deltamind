#pragma once
#include "GameObject.h"

class DisplayBoard : public GameObject {
protected:
	enum Type {
		Default,
		Move,
		Pause,
		Inventory,
		Items,
		Build,
		Shoot,
	};
	
	Type type;

	bool isVR;

	GameObject* lifter = nullptr;
	GameObject* screen = nullptr;

	void SetType(Type _type);

public:
	DisplayBoard() {}
	~DisplayBoard() {}

	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;
};

class DisplayBoard_Move : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Move);
	}
};
class DisplayBoard_Pause : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Pause);
	}
};
class DisplayBoard_Inventory : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Inventory);
	}
};
class DisplayBoard_Items : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Items);
	}
};
class DisplayBoard_Build : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Build);
	}
};
class DisplayBoard_Shoot : public DisplayBoard {
	void Awake(Object* obj) override {
		DisplayBoard::Awake(obj);
		SetType(Type::Shoot);
	}
};