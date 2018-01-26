#pragma once
#include <vector>
#include <IComponentManager.h>
#include <directxmath.h>
#include "ComponentBase.h"

struct animJoint {
	DirectX::XMFLOAT4X4 transform;
	int parent_index;
};

struct keyframe {
	double endTime;
	std::vector<animJoint> joints;
};

struct bindpose {
	const char * filePath;
	std::vector<animJoint> joints;
};

struct Animation {
	unsigned int animID;
	double duration;
	std::vector<keyframe> keyframes;
	bindpose* bPose;
};

struct AnimComponent: ComponentBase {
	double timePos;
	Animation * anim;
	std::vector<animJoint> tweens;
};

class AnimationManager : public IComponentManager {
	std::vector<bindpose> bindPoses;
	std::vector<Animation> animations;

	Animation* LoadAnimation(const char* _animationFilePath, const char* _bindposeFilePath);
	bindpose* LoadBindpose(const char* _bindposeFilePath);
public:
	AnimationManager();
	~AnimationManager();

	unsigned int AddElement(const char* _animationFilePath, const char* _bindposeFilePath);
	AnimComponent * GetElement(const unsigned int _id);
	void ResetComponent(ComponentBase * reset) override;


};
