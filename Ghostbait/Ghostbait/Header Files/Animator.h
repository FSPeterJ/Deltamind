#pragma once
#include "AnimationManager.h"
#include "ComponentBase.h"
#include <unordered_map>

namespace DirectX {
	
	struct XMFLOAT3X3;
	struct XMFLOAT4X4;
}

class Animator: public InstantiatedCompBase {
	unsigned int updateID;
	AnimationManager* animMan;
	std::unordered_map<std::string, Animation*> animations;
	std::unordered_map<std::string, int> jointPointers; //Used for quick searches for named joints
	Animation* currAnim = nullptr;
	double timePos = 0.0f;
	std::vector<animJoint> tweens = std::vector<animJoint>();
	DirectX::XMFLOAT3X3 pullRotation(DirectX::XMFLOAT4X4 pullFrom);
	DirectX::XMFLOAT3X3 lerpRotation(DirectX::XMFLOAT3X3 m1, DirectX::XMFLOAT3X3 m2, float ratio);
public:
	Animator();
	~Animator();

	void Destroy();
	void Initialize(AnimationManager* animManIn);
	void Update();
	void SetTime(float _timePos);
	void addAnim(const char* animFilePath, const char* bindposeFilePath, const char* animName);
	bool setState(const char* animName);

	void Copy(const Animator* that);
	double getTimePos() { return timePos; }
	std::unordered_map<std::string, Animation*>* getAnimations() { return &animations; }
	const std::vector<animJoint>* getTweens();
	Animation* getCurrentAnimation() { return currAnim; }
	DirectX::XMFLOAT4X4* getJointByName(const std::string& name);
	void ManipulateJointByName(const std::string &name, const DirectX::XMFLOAT4X4  &transform);
	void ManipulateJoint(animJoint* animationJoint, const DirectX::XMFLOAT4X4& _transformation);
};
