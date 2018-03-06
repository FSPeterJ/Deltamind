#pragma once
#include "Item.h"



#include "PhysicsExtension.h"

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

	ArcObject buildArc;

	void SpawnProjection();
	void Spawn();
	void RemoveProjection();
	void Remove();

	bool Snap(GameObject** obj);
	bool Snap(DirectX::XMFLOAT2* pos);
	bool SetObstacle(DirectX::XMFLOAT2 pos, bool active);
public:
	BuildTool();

	void SetPrefabs(std::vector<unsigned> prefabIDs);
	void Enable(bool onEndDestroy);
	void Disable();
	void Update();

	void Projection();
	void Activate();

	void CycleForward();
	void CycleBackward();

	void InactiveUpdate();
	void ActiveUpdate();

	void DeSelected();
	void Selected() override;
};
