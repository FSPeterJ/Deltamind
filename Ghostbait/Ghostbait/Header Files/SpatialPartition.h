#pragma once
#include <unordered_map>
#include <vector>
#include "GameObject.h"

struct Collider;
struct PhysicsComponent;

struct Unit {
	std::vector<PhysicsComponent*> components;
	
	Unit();
	Unit(PhysicsComponent* comp);
	int FindComponent(PhysicsComponent* comp);
	bool AddComponent(PhysicsComponent* comp);
	bool RemoveComponent(PhysicsComponent* comp);
};

class SpatialPartition {
	enum PositionOption {
		Both,
		Current,
		Previous,
	};

	uint32_t bucketCount = 1024;
	float unitSize = 0;
	std::unordered_map<uint32_t, Unit> table;
	uint32_t Hash(const float x, const float y, const float z);

public:
	SpatialPartition();
	SpatialPartition(uint32_t _bucketCount, float _unitSize);

	bool AddComponent(PhysicsComponent* component);
	bool RemoveComponent(PhysicsComponent* component, PositionOption option = Both);
	bool UpdateComponent(PhysicsComponent* component);

	const std::vector<PhysicsComponent*> GetComponentsFromUnit(const float x, const float y, const float z) const;
	const std::vector<PhysicsComponent*> GetComponentsFromUnit(const Unit u) const;
	const std::vector<Unit*> GetUnitsFromComponent(const PhysicsComponent* component);
};