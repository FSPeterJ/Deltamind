#include "SpatialPartition.h"
#include "PhysicsComponent.h"

Unit::Unit() {

}
Unit::Unit(PhysicsComponent* comp) {
	AddComponent(comp);
}
int Unit::FindComponent(PhysicsComponent* comp) {
	for (int i = 0; i < components.size(); ++i) {
		//TODO May need to overload the == operator
		if (comp == components[i]) {
			return i;
		}
	}
	return -1;
}
bool Unit::AddComponent(PhysicsComponent* comp) {
	if (FindComponent(comp) >= 0) {
		components.push_back(comp);
		return true;
	}
	return false;
}
bool Unit::RemoveComponent(PhysicsComponent* comp) {
	int index = FindComponent(comp);
	if (index >= 0) {
		components.erase(components.cbegin + index);
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

bool SpatialPartition::AddComponent(PhysicsComponent* component) {
	for (int i = 0; i < component->colliders.size(); ++i) {
		//TODO: This only works for set points. Doesnt take collider radius or rotation
		//		into account.
		float x = component->parentObject->position._41 + component->colliders[i].centerOffset.x;
		float y = component->parentObject->position._42 + component->colliders[i].centerOffset.y;
		float z = component->parentObject->position._43 + component->colliders[i].centerOffset.z;
		uint32_t index = Hash(x, y, z);
		
		if (table.find(index) != table.end()) table[index].AddComponent(component);
		else table[index] = Unit(component);
	}
}
bool SpatialPartition::RemoveComponent(PhysicsComponent* component, PositionOption option) {
	bool foundAndRemoved = false;
	uint32_t index1 = -1;
	for (int i = 0; i < component->colliders.size(); ++i) {
		//TODO: This only works for set points. Doesnt take collider radius or rotation
		//		into account.
		float x = component->colliders[i].centerOffset.x;
		float y = component->colliders[i].centerOffset.y;
		float z = component->colliders[i].centerOffset.z;
	
		if (option == Both || option == Previous) {
			 index1 = Hash(x + component->parentObject->position.Previous()._41,
								  y + component->parentObject->position.Previous()._42,
								  z + component->parentObject->position.Previous()._43);
			if (table.find(index1) != table.end()) {
				if (table[index1].RemoveComponent(component)) {
					foundAndRemoved = true;
				}
			}
		}
		if (option == Both || option == Current) {
			uint32_t index2 = Hash(x + component->parentObject->position._41,
								  y + component->parentObject->position._42,
								  z + component->parentObject->position._43);
			if (index2 != index1) {
				if (table.find(index2) != table.end()) {
					if (table[index2].RemoveComponent(component)) {
						foundAndRemoved = true;
					}
				}
			}
		}
	}
	return foundAndRemoved;
}
bool SpatialPartition::UpdateComponent(PhysicsComponent* component) {
	if (component->parentObject->position != component->parentObject->position.Previous()) {
		if (RemoveComponent(component, Previous)) {
			AddComponent(component);
			return true;
		}
		else return false;
	}
	return true;
}
const std::vector<PhysicsComponent*> SpatialPartition::GetComponentsFromUnit(const float x, const float y, const float z) const {

}
const std::vector<PhysicsComponent*> SpatialPartition::GetComponentsFromUnit(const Unit u) const {
	return u.components;
}
const std::vector<Unit*> SpatialPartition::GetUnitsFromComponent(const PhysicsComponent* component) {
	std::vector<Unit*> units;
	for (int i = 0; i < component->colliders.size(); ++i) {
		//TODO: This only works for set points. Doesnt take collider radius or rotation
		//		into account.
		float x = component->parentObject->position._41 + component->colliders[i].centerOffset.x;
		float y = component->parentObject->position._42 + component->colliders[i].centerOffset.y;
		float z = component->parentObject->position._43 + component->colliders[i].centerOffset.z;
		uint32_t index = Hash(x, y, z);
	}
	
	return table[]
}