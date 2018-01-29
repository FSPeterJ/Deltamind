#include "AnimatorManager.h"
#include "Object.h"

AnimatorManager::~AnimatorManager()
{
}

Animator * AnimatorManager::CreateElement()
{
	Animator* toReturn = animators.ActivateMemory();
	toReturn->Initialize(animMan);
	return toReturn;
}

void AnimatorManager::RemoveElement(EventMessageBase * e)
{
	DestroyMessage* removeable = (DestroyMessage*)e;
	if (removeable->RetrieveObject()->GetComponent<Animator>())
		animators.DeactivateMemory(removeable->RetrieveObject()->GetComponent<Animator>());
}
