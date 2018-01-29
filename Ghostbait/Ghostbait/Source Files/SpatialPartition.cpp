#include "SpatialPartition.h"
#include "PhysicsComponent.h"
Unit::Unit() {

}
Unit::Unit(PhysicsComponent* comp) {
	AddComponent(comp);
}
bool Unit::HasComponent(PhysicsComponent* comp) {
	for (int i = 0; i < components.size(); ++i) {
		//TODO May need to overload the == operator
		if (comp == components[i]) {
			return true;
		}
	}
	return false;
}
void Unit::AddComponent(PhysicsComponent* comp) {
	components.push_back(comp);
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

void SpatialPartition::AddComponent(PhysicsComponent* component) {
	for (int i = 0; i < component->colliders.size(); ++i) {
		//TODO: This only works for set positions. Doesnt take collider radius or
		//		anything into account.
		float x = component->parentObject->position._41 + component->colliders[i].centerOffset.x;
		float y = component->parentObject->position._42 + component->colliders[i].centerOffset.y;
		float z = component->parentObject->position._43 + component->colliders[i].centerOffset.z;
		uint32_t index = Hash(x, y, z);
		if (table.find(index) == table.end()) {
			if (!table[index].HasComponent(component)) {
				table[index].AddComponent(component);
			}
		}
		else table[index] = Unit(component);
	}
}
void SpatialPartition::UpdateComponent(PhysicsComponent* component) {
	//if(Hash(component))
}
void SpatialPartition::RemoveComponent(PhysicsComponent* component) {

}