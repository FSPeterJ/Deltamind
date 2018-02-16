#pragma once
#include "Item.h"


class ControllerObject;

class BuildTool : public Item {
	struct BuildItem {
		GameObject* object;
		int ID;
	};
	enum Mode {
		SPAWN,
		REMOVE,
	};

	int currentPrefabIndex = 0;
	std::vector<BuildItem> prefabs;
	std::vector<GameObject*> builtItems;
	Mode currentMode = SPAWN;
	DirectX::XMFLOAT3 spawnPos;
	GameObject* currentlySelectedItem = nullptr;
	ControllerObject* parent;

	void SpawnProjection();
	void Spawn();
	void RemoveProjection();
	void Remove();
public:
	BuildTool();
	BuildTool(std::vector<int> prefabIDs);

	void SetPrefabs(std::vector<int> prefabIDs);
	void SetParent(ControllerObject* _parent);

	void Projection();
	void Activate();

	void CycleForward();
	void CycleBackward();

	void InactiveUpdate();
	void ActiveUpdate();
};
