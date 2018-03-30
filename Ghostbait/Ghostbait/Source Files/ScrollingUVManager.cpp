#include "ScrollingUVManager.h"
#include "TypeMapping.h"

ScrollingUVManager::ScrollingUVManager() {
	TypeMap::RegisterComponentAlias<ScrollingUV>("Scroll");
}
ScrollingUVManager::~ScrollingUVManager() {

}

void ScrollingUVManager::Update() {
	std::vector<ScrollingUV*>* activeList = pooledUVs.GetActiveList();
	for (int i = 0; i < (int)activeList->size(); ++i) {
		ScrollingUV* element = (*activeList)[i];
		element->offset.x += element->velocity.x;
		element->offset.y += element->velocity.y;
	}
}

ScrollingUV* ScrollingUVManager::GetReferenceComponent(const char* _FilePath, const char* _data) {
	ScrollingUV* prefab = pooledUVs.ActivateMemory();
	
	//Use _data to fill prefab's data
	memcpy((char*)&prefab->velocity, _data, 8);
	
	
	return prefab;
}

ScrollingUV* ScrollingUVManager::CloneComponent(ComponentBase* referece) {
	ScrollingUV* clone = pooledUVs.ActivateMemory();
	ScrollingUV* source = (ScrollingUV*)referece;

	clone->offset = source->offset;
	clone->velocity = source->velocity;

	return clone;
}

void ScrollingUVManager::ResetComponent(ComponentBase* reset) {
	pooledUVs.DeactivateMemory(reset);
}