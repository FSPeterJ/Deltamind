#include "SpatialPartition.h"
#include "PhysicsComponent.h"
#include "Console.h"

SpatialPartition::Unit::Unit() {
}
SpatialPartition::Unit::Unit(PhysicsComponent* comp) {
	AddComponent(comp);
}
std::vector<PhysicsComponent*>::iterator SpatialPartition::Unit::FindComponent(PhysicsComponent* comp) {
	//for(uint32_t i = 0; i < (uint32_t) components.size(); ++i) {
	//	//TODO Does this comparison work?
	//	// pointer comparison is fine
	//	// std::find would probably be better than int interation
	//	if(comp == components[i]) {
	//		return i;
	//	}
	//}
	//return -1;

	return std::find(components.begin(), components.end(), comp);
}
bool SpatialPartition::Unit::AddComponent(PhysicsComponent* comp) {
	//Is component in here already?
	if(FindComponent(comp) == components.end()) {
		//Add it
		components.push_back(comp);
		return true;
	}
	return false;
}
bool SpatialPartition::Unit::RemoveComponent(PhysicsComponent* comp) {
	//if(index >= 0) {
	//	//TODO: Does this work like I expect?
	//	// Note: This means it will be impossible to call RemoveComponent while iterating components in this context, which is probably not an issue
	//	// Swap and pop is a faster method of removal, as vector by default memmoves the entire array down one.
	//	components.erase(components.begin() + (int)index);
	//	return true;
	//}
	//return false;
	std::vector<PhysicsComponent*>::iterator index = FindComponent(comp);
	if (index == components.end()) return false;

	std::iter_swap(index, components.end() - 1);
	components.pop_back();
	return true;
}

SpatialPartition::SpatialPartition() {
	bucketCount = 1024;
	unitSize = PARTITION_UNIT_SIZE;
}
SpatialPartition::SpatialPartition(uint32_t _bucketCount, float _unitSize) : bucketCount(_bucketCount), unitSize(_unitSize) {
}

uint32_t SpatialPartition::Hash(const float x, const float y, const float z) {
	const int h1 = 0x8da6b343;	// Arbitrary, large primes.
	const int h2 = 0xd8163841;	// Primes are popular for hash functions
	const int h3 = 0xcb1ab31f;	// for reducing the chance of hash collision.
	int cellX = (int)(x / unitSize);
	int cellY = (int)(y / unitSize);
	int cellZ = (int)(z / unitSize);
	uint32_t n = (uint32_t)(h1*cellX + h2*cellY + h3*cellZ);
	n = n % bucketCount;	// Wrap indices to stay in bucket range
	//Console::WriteLine << cellX << ", " << cellY << ", " << cellZ << "  bucket: " << n;
	if (n < 0) n += bucketCount;	// Keep indices in positive range
	return n;
}
uint32_t SpatialPartition::Hash(DirectX::XMFLOAT3 point) {
	return Hash(point.x, point.y, point.z);
}
std::vector<uint32_t> SpatialPartition::Hash(const AABB aabb) {
	using namespace DirectX;

	std::vector<uint32_t> indicies;
	std::vector<XMFLOAT3> points;

	if (aabb.isLarge) {
		//XMFLOAT3 toStore;
		//XMVECTOR width = XMLoadFloat3(&points[4]) - XMLoadFloat3(&points[0]), 
		//				 height = XMLoadFloat3(&points[2]) - XMLoadFloat3(&points[0]), 
		//				 length = XMLoadFloat3(&points[1]) - XMLoadFloat3(&points[0]); 
		//float widthSq = XMVectorGetX(XMVector3LengthSq(width)),
		//	 heightSq = XMVectorGetX(XMVector3LengthSq(height)),
		//	 lengthSq = XMVectorGetX(XMVector3LengthSq(length));
		//float unitSizeSq = unitSize * unitSize;

		//float widthRatio = 1.0f, heightRatio = 1.0f, lengthRatio = 1.0f;

		//if (widthSq > unitSizeSq) widthRatio = unitSize / sqrtf(widthSq);
		//if (heightSq > unitSizeSq) heightRatio = unitSize / sqrtf(heightSq);
		//if (lengthSq > unitSizeSq) lengthRatio = unitSize / sqrtf(lengthSq);

		//if (widthRatio < 1.0f || heightRatio < 1.0f || lengthRatio < 1.0f) {
		//	points.reserve(64);
		//	XMVECTOR currY, currZ;
		//	for (float yChange = 0.0f - heightRatio; yChange < 1.0f;) {
		//		yChange += heightRatio;
		//		if (yChange >= 1.0f) yChange = 1.0f;
		//		currY = XMLoadFloat3(&points[0]) + (height * yChange);
		//		for (float zChange = 0.0f - lengthRatio; zChange < 1.0f;) {
		//			zChange += lengthRatio;
		//			if (zChange >= 1.0f) zChange = 1.0f;
		//			currZ = currY + (length * zChange);
		//			for (float xChange = 0.0f - widthRatio; xChange < 1.0f;) {
		//				xChange += widthRatio;
		//				if (xChange >= 1.0f) xChange = 1.0f;
		//				XMStoreFloat3(&toStore, currZ + (width * xChange));
		//				points.push_back(toStore);
		//			}
		//		}
		//	}
		//}

		float width = aabb.max.x - aabb.min.x,
			height = aabb.max.y - aabb.min.y,
			length = aabb.max.z - aabb.min.z;

		float widthRatio = 1.0f, heightRatio = 1.0f, lengthRatio = 1.0f;

		if (width > unitSize) widthRatio = unitSize / width;
		if (height > unitSize) heightRatio = unitSize / height;
		if (length > unitSize) lengthRatio = unitSize / length;

		if (widthRatio < 1.0f || heightRatio < 1.0f || lengthRatio < 1.0f) {
			points.reserve(64);
			float currX, currY, currZ;
			for (float yChange = 0.0f - heightRatio; yChange < 1.0f;) {
				yChange += heightRatio;
				if (yChange >= 1.0f) yChange = 1.0f;
				currY = aabb.min.y + (height * yChange);
				for (float zChange = 0.0f - lengthRatio; zChange < 1.0f;) {
					zChange += lengthRatio;
					if (zChange >= 1.0f) zChange = 1.0f;
					currZ = aabb.min.z + (length * zChange);
					for (float xChange = 0.0f - widthRatio; xChange < 1.0f;) {
						xChange += widthRatio;
						if (xChange >= 1.0f) xChange = 1.0f;
						currX = aabb.min.x + (width * xChange);
						points.push_back({ currX, currY, currZ });
					}
				}
			}
		}
	}
	else {
		points.resize(8);
		points[0] = aabb.min;
		points[1] = { aabb.min.x, aabb.min.y, aabb.max.z };
		points[2] = { aabb.min.x, aabb.max.y, aabb.min.z };
		points[3] = { aabb.min.x, aabb.max.y, aabb.max.z };
		points[4] = { aabb.max.x, aabb.min.y, aabb.min.z };
		points[5] = { aabb.max.x, aabb.min.y, aabb.max.z };
		points[6] = { aabb.max.x, aabb.max.y, aabb.min.z };
		points[7] = aabb.max;
	}
	
	int index;
	for(size_t point = 0; point < points.size(); ++point) {
		index = Hash(points[point]);
		bool found = false;
		//for (unsigned int exist = 0; exist < indicies.size(); ++exist) {
		//	if (index == indicies[exist]) {
		//		found = true;
		//		break;
		//	}
		//}
		if(std::find(indicies.begin(), indicies.end(), index) == indicies.end()) indicies.push_back(index);
	}
	return indicies;
}

bool SpatialPartition::AddComponent(PhysicsComponent* component) {
	bool anythingAdded = false;
	std::vector<uint32_t> indicies = Hash(component->currentAABB);
	//Console::WriteLine << component->parentObject << " occupies " << indicies.size() << " buckets";
	for (unsigned int i = 0; i < indicies.size(); ++i) {
		if (table.find(indicies[i]) != table.end()) {
			if (table[indicies[i]].AddComponent(component)) {
				anythingAdded = true;
			}
		} else {
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
		for (unsigned int i = 0; i < indicies.size(); ++i) {
			if (table.find(indicies[i]) != table.end()) {
				if (table[indicies[i]].RemoveComponent(component)) {
					foundAndRemoved = true;
					if (table[indicies[i]].components.size() <= 0)
						table.erase(indicies[i]);
				}
			}
		}
	} else if(RemoveComponent(component, Previous) || RemoveComponent(component, Current)) {
		foundAndRemoved = true;
	}
	return foundAndRemoved;
}
void SpatialPartition::UpdateComponent(PhysicsComponent* component) {
	//Did it move?
	if(component->currentAABB != component->previousAABB) {
		//Remove previous component instances
		RemoveComponent(component, Previous);
		//Add Current component instances
		AddComponent(component);
		//set previous to current
		component->previousAABB = component->currentAABB;
	}
}

const std::vector<PhysicsComponent*> SpatialPartition::GetComponentsToTest() {
	//std::vector<PhysicsComponent*> testComps;
	//std::vector<uint32_t> indicies = Hash(component->currentAABB);
	////Console::WriteLine << (int)indicies.size();
	////for every bin
	//for (unsigned int index = 0; index < indicies.size(); ++index) {
	//	//for every component in that bin
	//	for (unsigned int newComp = 0; newComp < table[indicies[index]].components.size(); ++newComp) {
	//		//is it a dupe, or do I add it?
	//		if (table[indicies[index]].components[newComp] == component)
	//			continue;

	//		bool found = false;
	//		for (unsigned int oldComp = 0; oldComp < testComps.size(); ++oldComp) {
	//			if (table[indicies[index]].components[newComp] == testComps[oldComp]) {
	//				found = true;
	//				break;
	//			}
	//		}
	//		if(!found) testComps.push_back(table[indicies[index]].components[newComp]);
	//	}
	//}
	//return testComps;

	std::vector<PhysicsComponent*> testComps;

	for each (const auto &bucket in table)
	{
		
		if (bucket.second.components.size() > 1) {
			for (unsigned int i = 0; i < bucket.second.components.size(); ++i) {
				assert(bucket.second.components[i]  != nullptr);
				testComps.push_back(bucket.second.components[i]);
				//Console::WriteLine << "Bucket: " << bucket.first << "  Size: " << bucket.second.components.size();
			}
			testComps.push_back(nullptr);
		}
	}
	return testComps;
}

const std::vector<PhysicsComponent*> SpatialPartition::GetComponentsToTest(uint32_t hashIndex) {
	std::vector<PhysicsComponent*> toReturn; 
	if(table.find(hashIndex) != table.end())
		return table[hashIndex].components;
	return toReturn;
}

const uint32_t SpatialPartition::GetHashedIndex(DirectX::XMFLOAT3 position) {
	return Hash(position);
}

