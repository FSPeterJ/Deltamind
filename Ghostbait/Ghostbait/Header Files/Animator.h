#pragma once
#include "AnimationManager.h"
#include "ComponentBase.h"
#include <unordered_map>
#include "PhysicsRegistry.h"

namespace DirectX {
	
	struct XMFLOAT3X3;
	struct XMFLOAT4X4;
}

class Animator: public InstantiatedCompBase {
	unsigned int updateID = 0;
	AnimationManager* animMan;
	std::unordered_map<std::string, Animation*> animations;
	std::unordered_map<std::string, int> jointPointers; //Used for quick searches for named joints
	Animation* currAnim = nullptr;
	double timePos = 0.0;
	double currScale = 1.0;
	std::vector<animJoint> tweens = std::vector<animJoint>();
	DirectX::XMFLOAT3X3 pullRotation(DirectX::XMFLOAT4X4 pullFrom);
	DirectX::XMFLOAT3X3 lerpRotation(DirectX::XMFLOAT3X3 m1, DirectX::XMFLOAT3X3 m2, float ratio);
public:
	Animator();
	~Animator();

	void Destroy();
	void Disable();
	void Enable();
	void Initialize(AnimationManager* animManIn);
	void Update();
	void SetTime(double _timePos);
	void addAnim(const char* animFilePath, const char* bindposeFilePath, const char* animName);
	bool setState(const char* animName, double speed = 1.0);

	void Copy(const Animator* that);
	double getTimePos() { return timePos; }
	std::unordered_map<std::string, Animation*>* getAnimations() { return &animations; }
	const std::vector<animJoint>* getTweens();
	const double GetDuration();
	void SetJointMatrix(const int jointIndex, const DirectX::XMFLOAT4X4& mat);
	const DirectX::XMFLOAT4X4& GetJointMatrix(const int jointIndex) const;

	Animation* getCurrentAnimation() { return currAnim; }
	DirectX::XMFLOAT4X4* getJointByName(const std::string& name);
	void ManipulateJointByName(const std::string &name, const DirectX::XMFLOAT4X4  &transform);
	void ManipulateJointByName(const std::string& name, const DirectX::XMMATRIX& _transformation);
	void ManipulateJoint(animJoint* animationJoint, const DirectX::XMFLOAT4X4& _transformation);
	void ManipulateJoint(animJoint* animationJoint, const DirectX::XMMATRIX& _transformation);
	void ManipulateChildrendJoints(animJoint* animationJoint, const DirectX::XMMATRIX& _transformation, const DirectX::XMMATRIX& animationJointParents);
};
