#pragma once
#include "Animator.h"
#include "Pool.h"
#include "IComponentManager.h"
#include "GameObject.h"

#define MAX_ANIMATORS 8192 //half the max objects as of writing this, I wasn't feeling reserving an absurd amount of memory for something that really doesn't need it
class AnimatorManager : public IComponentManager
{
	AnimationManager* animMan;
	Pool<Animator> animators = Pool<Animator>(MAX_ANIMATORS);
public:

	AnimatorManager(AnimationManager* animManIn) : animMan(animManIn) 
	{
		TypeMap::RegisterComponentAlias<Animator>("Animate");
	};
	~AnimatorManager();

	Animator* GetReferenceComponent(const char * _FilePath, const char* _data) override;
	Animator* CloneComponent(ComponentBase* reference) override;
	void ResetComponent(ComponentBase* reset) override;
};

