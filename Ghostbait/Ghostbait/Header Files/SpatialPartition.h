#pragma once
#include <unordered_map>
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "AABB.h"

class Unit {
	float x, y, z;
	float size;
	PhysicsComponent* obj;
};

class SpatialPartition {
	std::unordered_map<uint32_t, Unit> table;


};