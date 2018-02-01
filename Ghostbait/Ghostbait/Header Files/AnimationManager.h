#pragma once
#include <vector>
#include <map>

namespace DirectX {
	struct XMFLOAT4X4;
}
class ComponentBase;

#define MAX_ANIMATIONS 512
struct animJoint {
	DirectX::XMFLOAT4X4* transform;
	int parent_index;
};

struct keyframe {
	double endTime;
	std::vector<animJoint> joints;
};

struct bindpose {
	std::vector<animJoint> joints;
};

struct Animation {
	unsigned int animID;
	double duration;
	std::vector<keyframe> keyframes;
	bindpose* bPose;
};

class AnimationManager {
	std::vector<bindpose*> bindPoses;
	std::vector<Animation*> animations;
	std::map<std::string, Animation*> animNames;
	std::map<std::string, bindpose*> bindposeNames;

	Animation* LoadAnimation(const char* _animationFilePath, const char* _bindposeFilePath);
	bindpose* LoadBindpose(const char* _bindposeFilePath);
public:
	AnimationManager();
	~AnimationManager();

	void Destroy();
	unsigned int AddElement(const char* _animationFilePath, const char* _bindposeFilePath);
	Animation * GetElement(const unsigned int _id);
	Animation* GetReferenceAnimation(const char * _FilePath, const char * _bindposeFilePath);
	void ResetComponent(ComponentBase * reset);


};
