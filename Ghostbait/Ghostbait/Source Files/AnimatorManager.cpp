#include "AnimatorManager.h"

#include "TypeMapping.h"        // for TypeMap
AnimatorManager::~AnimatorManager()
{
}
AnimatorManager::AnimatorManager(AnimationManager* animManIn) : animMan(animManIn) {
	TypeMap::RegisterComponentAlias<Animator>("Animate");
};
Animator* AnimatorManager::GetReferenceComponent(const char * _FilePath, const char* _data)
{
	Animator* toReturn = animators.ActivateMemory();
	toReturn->Initialize(animMan);

	int currIndex = 0;
	int numOfAnims = 0;
	int strLength = 0;
	memcpy(&strLength, &_data[currIndex], sizeof(strLength));
	currIndex += sizeof(strLength);
	char* bindposePath;
	bindposePath = new char[strLength];
	memcpy(&bindposePath[0], &_data[currIndex], strLength);
	currIndex += strLength;
	memcpy(&numOfAnims, &_data[currIndex], sizeof(numOfAnims));
	currIndex += sizeof(numOfAnims);
	for (int i = 0; i < numOfAnims; ++i)
	{
		char* filePath, *name;
		memcpy(&strLength, &_data[currIndex], sizeof(strLength));
		currIndex += sizeof(strLength);
		filePath = new char[strLength];
		memcpy(&filePath[0], &_data[currIndex], strLength);
		currIndex += strLength;
		memcpy(&strLength, &_data[currIndex], sizeof(strLength));
		currIndex += sizeof(strLength);
		name = new char[strLength];
		memcpy(&name[0], &_data[currIndex], strLength);
		currIndex += strLength;
		toReturn->addAnim(filePath, bindposePath, name);
		delete[] filePath;
		delete[] name;
	}

	delete[] bindposePath;
	return toReturn;
}

Animator * AnimatorManager::CloneComponent(ComponentBase * reference)
{
	Animator* toReturn = animators.ActivateMemory();
	toReturn->Copy(((Animator*)reference));
	toReturn->Initialize(animMan);
	return toReturn;
}

void AnimatorManager::ResetComponent(ComponentBase * reset)
{
	((Animator*)reset)->Destroy();
	animators.DeactivateMemory(reset);
}
