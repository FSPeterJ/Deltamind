#pragma once
#include "AnimationManager.h"
#include "ComponentBase.h"
#include <DirectXMath.h>       // for XMFLOAT3X3, XMFLOAT4X4 (ptr only)

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

	Animator& operator=(const Animator& that);

	double getTimePos() { return timePos; }
	std::map<std::string, Animation*>* getAnimations() { return &animations; }
	std::vector<animJoint>* getTweens() { return &tweens; }
	Animation* getCurrentAnimation() { return currAnim; }
};

