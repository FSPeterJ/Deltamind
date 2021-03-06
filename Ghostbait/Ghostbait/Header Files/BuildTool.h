#pragma once
#include "Item.h"
#include "Light.h"

#include "PhysicsExtension.h"

class HexGrid;
class GameData;
struct Material;

class BuildTool : public Item {
	struct BuildItem {
		GameObject* object;
		int ID;
	};
	enum Mode {
		BUILD,
		REMOVE,
		REPAIR,
	};

	GameObject* gearDisplay = nullptr;
	GameObject* gearAdjustmentDisplay = nullptr;
	Material* gearMat = nullptr;
	Material* gearAdjustmentMat = nullptr;

	int currentPrefabIndex = 0;
	std::vector<BuildItem> prefabs;
	static std::vector<GameObject*> builtItems;
	Mode currentMode = BUILD;
	DirectX::XMFLOAT3 spawnPos;
	int currentlySelectedItemIndex = -1;
	GameObject* currentlySelectedItem = nullptr;
	GameObject* toDestroy = nullptr;
	int toDestroyIndex = -1;
	Light light;

	//Game values
	HexGrid* grid = nullptr;
	GameData* gameData = nullptr;

	void Spawn();
	void Remove();
	void Repair();
	void SpawnProjection();
	void RemoveProjection();
	void RepairProjection();

	bool Snap(GameObject** obj);
	bool Snap(DirectX::XMFLOAT2* pos);
	bool SetObstacle(DirectX::XMFLOAT2 pos, bool active);
	bool CanBuildHere(DirectX::XMFLOAT2& spawnPos);

	void SetColor(const char* colorVariant);
public:
	CastObject ray;
	//CastObject buildRay;
	//CastObject deleteRay;
	//CastObject repairRay;

	BuildTool();

	inline void SetGrid(HexGrid* _grid) { grid = _grid; };
	inline void SetGameData(GameData* _gameData) { gameData = _gameData; }
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
	void Destroy() override;
};
