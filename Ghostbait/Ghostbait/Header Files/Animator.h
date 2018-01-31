#pragma once
#include "AnimationManager.h"
#include <map>

class Animator : public InstantiatedCompBase
{
	AnimationManager* animMan;
	std::map<std::string, Animation*> animations;
	Animation* currAnim;
	double timePos;
	std::vector<animJoint> tweens;

	DirectX::XMFLOAT3X3 pullRotation(DirectX::XMFLOAT4X4 pullFrom);
	DirectX::XMFLOAT3X3 lerpRotation(DirectX::XMFLOAT3X3 m1, DirectX::XMFLOAT3X3 m2, float ratio);
public:
	Animator();
	~Animator();
	
	void Initialize(AnimationManager* animManIn) { animMan = animManIn; };
	void Update();
	void addAnim(const char* animFilePath, const char* bindposeFilePath, const char* animName);
	bool setState(const char* animName);

	Animator& operator=(const Animator& that)
	{
		if (this != &that)
		{
			//All pointer copies in this operator are not shallow; animators do not manage the memory they point to and should all point to the same places
			this->animMan = that.animMan;
			for (auto i = that.animations.begin(); i != that.animations.end(); ++i)
			{
				std::string name = i->first;
				Animation* point = i->second;
				this->animations[name] = point;
			}
			this->timePos = that.timePos;
			this->currAnim = that.currAnim;
			for (size_t i = 0; i < tweens.size(); ++i)
			{
				this->tweens.push_back(that.tweens[i]);
			}
		}
		return *this;
	};

	double getTimePos() { return timePos; }
	std::map<std::string, Animation*>* getAnimations() { return &animations; }
	std::vector<animJoint>* getTweens() { return &tweens; }
	Animation* getCurrentAnimation() { return currAnim; }
};

