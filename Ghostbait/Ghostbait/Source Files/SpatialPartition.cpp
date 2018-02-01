#include "SpatialPartition.h"
#include "PhysicsComponent.h"

SpatialPartition::Unit::Unit() {

}
SpatialPartition::Unit::Unit(PhysicsComponent* comp) {
	AddComponent(comp);
}
uint32_t SpatialPartition::Unit::FindComponent(PhysicsComponent* comp) {
	for (uint32_t i = 0; i < (uint32_t)components.size(); ++i) {
		//TODO Does this comparison work?
		if (comp == components[i]) {
			return i;
		}
	}
	return -1;
}
bool SpatialPartition::Unit::AddComponent(PhysicsComponent* comp) {
	//Is component in here already?
	if (FindComponent(comp) < 0) {
		//Add it
		components.push_back(comp);
		return true;
	}
	return false;
}
bool SpatialPartition::Unit::RemoveComponent(PhysicsComponent* comp) {
	int index = FindComponent(comp);
	if (index >= 0) {
		//TODO: Does this work like I expect?
		components.erase(components.begin() + index);
		return true;
	}
	return false;
}

SpatialPartition::SpatialPartition() {
	bucketCount = 1024;
	unitSize = 3;
}
SpatialPartition::SpatialPartition(uint32_t _bucketCount, float _unitSize) : bucketCount(_bucketCount), unitSize(_unitSize) {

}

uint32_t SpatialPartition::Hash(const float x, const float y, const float z) {
	const int h1 = 0x8da6b343;	// Arbitrary, large primes.
	const int h2 = 0xd8163841;	// Primes are popular for hash functions
	const int h3 = 0xcb1ab31f;	// for reducing the chance of hash collision.
	uint32_t n = (uint32_t)(h1*x + h2*y + h3*z);
	n = n % bucketCount;	// Wrap indices to stay in bucket range
	if (n < 0) n += bucketCount;	// Keep indices in positive range
	return n;
}
uint32_t SpatialPartition::Hash(DirectX::XMFLOAT3 point) {
	return Hash(point.x, point.y, point.z);
}
std::vector<uint32_t> SpatialPartition::Hash(const AABB aabb) {
	std::vector<uint32_t> indicies;
	std::vector<DirectX::XMFLOAT3> points;
	points.resize(8);
	points[0] = *aabb.min;
	points[1] = DirectX::XMFLOAT3(aabb.min->x, aabb.min->y, aabb.max->z);
	points[2] = DirectX::XMFLOAT3(aabb.min->x, aabb.max->y, aabb.min->z);
	points[3] = DirectX::XMFLOAT3(aabb.min->x, aabb.max->y, aabb.max->z);
	points[4] = DirectX::XMFLOAT3(aabb.max->x, aabb.min->y, aabb.min->z);
	points[5] = DirectX::XMFLOAT3(aabb.max->x, aabb.min->y, aabb.max->z);
	points[6] = DirectX::XMFLOAT3(aabb.max->x, aabb.max->y, aabb.min->z);
	points[7] = *aabb.max;
	int index;
	for (int point = 0; point < 8; ++point) {
		index = Hash(points[point]);
		bool found = false;
		for (int exist = 0; exist < indicies.size(); ++exist) {
			if (index == indicies[exist]) {
				found = true;
				break;
			}
		}
		if (!found) indicies.push_back(index);
	}
	return indicies;
}

bool SpatialPartition::AddComponent(PhysicsComponent* component) {
	bool anythingAdded = false;
	std::vector<uint32_t> indicies = Hash(component->currentAABB);
	for (int i = 0; i < indicies.size(); ++i) {
		if (table.find(indicies[i]) != table.end()) {
			if (table[indicies[i]].AddComponent(component)) {
				anythingAdded = true;
			}
		}
		else {
			table[indicies[i]] = Unit(component);
			anythingAdded = true;
		}
	}
	return anythingAdded;
}
bool SpatialPartition::RemoveComponent(PhysicsComponent* component, PositionOption option) {
	bool foundAndRemoved = false;
	std::vector<uint32_t> indicies;
	if (option != Both) {
		if (option == Current) indicies = Hash(component->currentAABB);
		else if (option == Previous) indicies = Hash(component->previousAABB);
		for (int i = 0; i < indicies.size(); ++i) {
			if (table.find(indicies[i]) != table.end()) {
				if (table[indicies[i]].RemoveComponent(component)) {
					foundAndRemoved = true;
				}
			}
		}
	}
	else if (RemoveComponent(component, Previous) || RemoveComponent(component, Current)) {
		foundAndRemoved = true;
	}
	return foundAndRemoved;
}
void SpatialPartition::UpdateComponent(PhysicsComponent* component) {
	//Did it move?
	if (component->currentAABB != component->previousAABB) {
		//Remove previous component instances
		RemoveComponent(component, Previous);
		//Add Current component instances
		AddComponent(component);
		//set previous to current
		component->previousAABB = component->currentAABB;
	}
}

const std::vector<PhysicsComponent*> SpatialPartition::GetComponentsToTest(const PhysicsComponent* component) {
	std::vector<PhysicsComponent*> testComps;
	std::vector<uint32_t> indicies = Hash(component->currentAABB);
	//for every bin
	for (int index = 0; index < indicies.size(); ++index) {
		//for every component in that bin
		for (int newComp = 0; newComp < table[indicies[index]].components.size(); ++newComp) {
			//is it a dupe, or do I add it?
			bool found = false;
			for (int oldComp = 0; oldComp < testComps.size(); ++oldComp) {
				if (table[indicies[index]].components[newComp] == testComps[oldComp] || 
					table[indicies[index]].components[newComp] == component) {
					found = true;
					break;
				}
			}
			if(!found) testComps.push_back(table[indicies[index]].components[newComp]);
		}
	}
	return testComps;
}
