#pragma once
#include "Item.h"
#include "ProgressBar.h"

class ControllerObject;

class BuildTool : public Item {
	struct BuildItem {
		GameObject* object;
		unsigned ID;
	};
	enum Mode {
		SPAWN,
		REMOVE,
	};

	unsigned currentPrefabIndex = 0;
	std::vector<BuildItem> prefabs;
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
	BuildTool(std::vector<unsigned> prefabIDs);

	void SetPrefabs(std::vector<unsigned> prefabIDs);
	void SetParent(ControllerObject* _parent);

	void Projection();
	void Activate();

	void CycleForward();
	void CycleBackward();

	void InactiveUpdate();
	void ActiveUpdate();
};
