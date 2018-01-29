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
	bool HasComponent(PhysicsComponent* comp);
	void AddComponent(PhysicsComponent* comp);
};

class SpatialPartition {
	uint32_t bucketCount = 1024;
	float unitSize = 0;
	std::unordered_map<uint32_t, Unit> table;
	uint32_t Hash(const float x, const float y, const float z);

public:
	SpatialPartition();
	SpatialPartition(uint32_t _bucketCount, float _unitSize);

	void AddComponent(PhysicsComponent* component);
	void UpdateComponent(PhysicsComponent* component);
	void RemoveComponent(PhysicsComponent* component);

	const std::vector<PhysicsComponent*> GetComponentsFromUnit(const float x, const float y, const float z) const;
	const std::vector<PhysicsComponent*> GetComponentsFromUnit(const Unit u) const;
	const Unit GetUnitFromComponent(const PhysicsComponent* component);
};