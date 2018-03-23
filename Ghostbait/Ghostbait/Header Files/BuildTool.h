#pragma once
#include "Item.h"

#include "PhysicsExtension.h"

class HexGrid;

class BuildTool : public Item {
	struct BuildItem {
		GameObject* object;
		int ID;
	};
	enum Mode {
		SPAWN,
		REMOVE,
	};

	GameObject* gearDisplay = nullptr;
	GameObject* gearAdjustmentDisplay = nullptr;
	bool adjustmentRender = true;

	int currentPrefabIndex = 0;
	std::vector<BuildItem> prefabs;
	std::vector<GameObject*> builtItems;
	Mode currentMode = SPAWN;
	DirectX::XMFLOAT3 spawnPos;
	int currentlySelectedItemIndex = -1;
	GameObject* currentlySelectedItem = nullptr;
	bool prevLocationValid = true;

	//Game values
	HexGrid* grid = nullptr;
	unsigned* gears = nullptr; 
	unsigned* turretsSpawned = nullptr;
	unsigned* maxTurrets = nullptr;

	void RenderAdjustmentDisplay(bool render);

	void SpawnProjection();
	void Spawn();
	void RemoveProjection();
	void Remove();

	bool Snap(GameObject** obj);
	bool Snap(DirectX::XMFLOAT2* pos);
	bool SetObstacle(DirectX::XMFLOAT2 pos, bool active);
	bool CanBuildHere(DirectX::XMFLOAT2& spawnPos);
public:
	CastObject buildArc;
	CastObject deleteRay;

	BuildTool();

	inline void SetGrid(HexGrid* _grid) { grid = _grid; };
	inline void SetGears(unsigned* _gears) { gears = _gears; };
	inline void SetTurretCap(unsigned* _turretsSpawned, unsigned* _maxTurrets) { turretsSpawned = _turretsSpawned; maxTurrets = _maxTurrets; };
	void SetPrefabs(std::vector<unsigned> prefabIDs);
	void Enable();
	void Disable();

	void Projection();
	void Activate();

	void CycleForward();
	void CycleBackward();

	void InactiveUpdate();
	void ActiveUpdate();

	void DeSelected();
	void Selected() override;

	void Awake(Object* obj) override;
};
