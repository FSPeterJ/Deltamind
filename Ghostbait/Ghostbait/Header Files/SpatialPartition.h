#pragma once
#include <unordered_map>
#include <vector>

namespace DirectX {
	struct XMFLOAT3;
}

struct PhysicsComponent;
struct AABB;

class SpatialPartition {
	struct Unit {
		std::vector<PhysicsComponent*> components;

		Unit();
		Unit(PhysicsComponent* comp);
		int64_t FindComponent(PhysicsComponent* comp);
		bool AddComponent(PhysicsComponent* comp);
		bool RemoveComponent(PhysicsComponent* comp);
	};

	enum PositionOption {
		Both,
		Current,
		Previous,
	};

	uint32_t bucketCount = 1024;
	float unitSize = 0;
	std::unordered_map<uint32_t, Unit> table;
	uint32_t Hash(const float x, const float y, const float z);
	uint32_t Hash(DirectX::XMFLOAT3 point);
	std::vector<uint32_t> Hash(const AABB aabb);

public:
	SpatialPartition();
	SpatialPartition(uint32_t _bucketCount, float _unitSize);

	bool AddComponent(PhysicsComponent* component);
	bool RemoveComponent(PhysicsComponent* component, PositionOption option = Both);
	void UpdateComponent(PhysicsComponent* component);

	const std::vector<PhysicsComponent*> GetComponentsToTest(const PhysicsComponent* component);
};
