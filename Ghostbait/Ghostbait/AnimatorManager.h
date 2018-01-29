#pragma once
#include "Animator.h"
#include "Pool.h"
#include "MessageEvents.h"

#define MAX_ANIMATORS 8192 //half the max objects as of writing this, I wasn't feeling reserving an absurd amount of memory for something that really doesn't need it
class AnimatorManager
{
	AnimationManager* animMan;
	Pool<Animator> animators;
public:

	AnimatorManager(AnimationManager* animManIn) : animMan(animManIn) 
	{
		animators = Pool<Animator>(MAX_ANIMATORS); 
		MessageEvents::Subscribe(EVENT_Destroy, [this](EventMessageBase * _e) {this->RemoveElement(_e); });
	};
	~AnimatorManager();

	Animator* CreateElement();
	void RemoveElement(EventMessageBase* e);
};

