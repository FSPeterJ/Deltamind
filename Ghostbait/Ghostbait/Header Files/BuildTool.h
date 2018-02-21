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

	void SpawnProjection();
	void Spawn();
	void RemoveProjection();
	void Remove();

	bool Snap(GameObject** obj);
	bool Snap(DirectX::XMFLOAT2* pos);
	bool SetObstacle(DirectX::XMFLOAT2 pos, bool active);
public:
	BuildTool();
	BuildTool(std::vector<unsigned> prefabIDs);
	BuildTool(std::vector<int> prefabIDs);

	void SetPrefabs(std::vector<unsigned> prefabIDs);

	void Projection();
	void Activate();

	void CycleForward();
	void CycleBackward();

	void InactiveUpdate();
	void ActiveUpdate();

	void HideBuildItems();
};
