#include "AABB.h"

bool AABB::operator==(const AABB& other) {
	if(min.x != other.min.x) return false;
	if(min.y != other.min.y) return false;
	if(min.z != other.min.z) return false;
	if(max.x != other.max.x) return false;
	if(max.y != other.max.y) return false;
	if(max.z != other.max.z) return false;
	return true;
}
bool AABB::operator!=(const AABB& other) {
	if(*this == other) return false;
	return true;
}
AABB AABB::operator=(const AABB& other) {
	this->max = other.max;
	this->min = other.min;
	this->isLarge = other.isLarge;
	return *this;
}

AABB::AABB(DirectX::XMFLOAT3& _min, DirectX::XMFLOAT3& _max) {
	min = _min;
	max = _max;
	isLarge = (max.x - min.x > PARTITION_UNIT_SIZE) || (max.y - min.y > PARTITION_UNIT_SIZE) || (max.z - min.z > PARTITION_UNIT_SIZE);
}

