#include "Animator.h"
#include "GhostTime.h"
#include "DebugRenderer.h"
#include "EngineStructure.h"
#include "AnimatorStructs.h"

using namespace DirectX;

DirectX::XMFLOAT3X3 Animator::pullRotation(DirectX::XMFLOAT4X4 pullFrom) {
	DirectX::XMFLOAT3X3 ret;
	ret._11 = pullFrom._11;
	ret._12 = pullFrom._12;
	ret._13 = pullFrom._13;
	ret._21 = pullFrom._21;
	ret._22 = pullFrom._22;
	ret._23 = pullFrom._23;
	ret._31 = pullFrom._31;
	ret._32 = pullFrom._32;
	ret._33 = pullFrom._33;
	return ret;
}

DirectX::XMFLOAT3X3 Animator::lerpRotation(DirectX::XMFLOAT3X3 m1, DirectX::XMFLOAT3X3 m2, float ratio) {
	DirectX::XMVECTOR v1 = XMQuaternionRotationMatrix(XMLoadFloat3x3(&m1));
	DirectX::XMVECTOR v2 = XMQuaternionRotationMatrix(XMLoadFloat3x3(&m2));
	DirectX::XMVECTOR lerpResult = DirectX::XMQuaternionSlerp(v1, v2, ratio);
	DirectX::XMFLOAT3X3 result;
	XMStoreFloat3x3(&result, DirectX::XMMatrixRotationQuaternion(lerpResult));
	return result;
}

Animator::Animator() {}

Animator::~Animator() {}

void Animator::Copy(const Animator * that) {
	if(this != that) {
		//Most pointer copies in this operator are not shallow; animators do not manage the memory they point to and should all point to the same places
		this->animMan = that->animMan;
		for(auto i = that->animations.begin(); i != that->animations.end(); ++i) {
			std::string name = i->first;
			Animation* point = i->second;
			this->animations[name] = point;
		}
		this->timePos = that->timePos;
		this->currAnim = that->currAnim;
		for(size_t i = 0; i < that->tweens.size(); ++i) {
			animJoint jointToPush = that->tweens[i];
			this->tweens.push_back(jointToPush);
			this->jointPointers[jointToPush.name] = (int)i;
		}
	}
}

const std::vector<animJoint>* Animator::getTweens() {
	return &tweens;
}

void Animator::SetJointMatrix(const int jointIndex, const DirectX::XMFLOAT4X4& mat) {
	tweens[jointIndex].transform = mat;
}

const DirectX::XMFLOAT4X4& Animator::GetJointMatrix(const int jointIndex) const {
	return tweens[jointIndex].transform;
}


DirectX::XMFLOAT4X4* Animator::getJointByName(const std::string &name) {
	//Accessing a map with [] without certantity that the map contains x is undefined behavior (The map will create a null value with key x since it does not exist)
	std::unordered_map<std::string, int>::iterator check = jointPointers.find(name);
	if(check == jointPointers.end()) {
		return nullptr;
	}
	return &tweens[(*check).second].transform;
}


//TODO: Manipulate joint by NOT STRING option
void Animator::ManipulateJointByName(const std::string &name, const DirectX::XMFLOAT4X4 &_transformation) {
	std::unordered_map<std::string, int>::iterator check = jointPointers.find(name);
	if(check != jointPointers.end()) {
		animJoint* animationJoint = &tweens[(*check).second];
		XMMATRIX transformation = XMLoadFloat4x4(&_transformation);
		while(true) {
			XMMATRIX transform = XMLoadFloat4x4(&animationJoint->transform);
			transform = transformation * transform;
			XMStoreFloat4x4(&animationJoint->transform, transform);
			// It should be possible to do this better?  I mean there is do while but... is that really better?
			if(animationJoint->child_count > 0) {
				transform.r[3] = transformation.r[3];
				for(int i = 0; i < animationJoint->child_count; ++i) {
					ManipulateChildrendJoints(&tweens[animationJoint->child_index[i]], transformation, transform);
				}
				break;
			}
			else {
				break;
			}
		}
	}
}

//TODO: Manipulate joint by NOT STRING option
void Animator::ManipulateJointByName(const std::string &name, const DirectX::XMMATRIX &_transformation) {
	std::unordered_map<std::string, int>::iterator check = jointPointers.find(name);
	if(check != jointPointers.end()) {
		animJoint* animationJoint = &tweens[(*check).second];
		XMMATRIX transformation = _transformation;
		while(true) {
			XMMATRIX transform = XMLoadFloat4x4(&animationJoint->transform);
			transform = transformation * transform;
			XMStoreFloat4x4(&animationJoint->transform, transform);
			// It should be possible to do this better?  I mean there is do while but... is that really better?
			if(animationJoint->child_count > 0) {
				//transform.r[3] = transformation.r[3];
				for(int i = 0; i < animationJoint->child_count; ++i) {
					ManipulateChildrendJoints(&tweens[animationJoint->child_index[i]], transformation, transform);
				}
				break;
			}
			else {
				break;
			}
		}
	}
}

void Animator::ManipulateJoint(animJoint* animationJoint, const DirectX::XMFLOAT4X4 &_transformation) {

	XMMATRIX transformation = XMLoadFloat4x4(&_transformation);
	while(true) {
		XMMATRIX transform = XMLoadFloat4x4(&animationJoint->transform);

		transform = transform * transformation;
		XMStoreFloat4x4(&animationJoint->transform, transform);
		// It should be possible to do this better?  I mean there is do while but... is that really better?
		if(animationJoint->child_count > 0) {

			transform.r[3] = transformation.r[3];
			for(int i = 0; i < animationJoint->child_count; ++i) {
				ManipulateJoint(&tweens[animationJoint->child_index[i]], transform);
			}
			break;
		}
		else {
			break;
		}
	}
}





void Animator::ManipulateJoint(animJoint* animationJoint, const DirectX::XMMATRIX &_transformation) {

	XMMATRIX transformation = _transformation;
	while(true) {
		XMMATRIX transform = XMLoadFloat4x4(&animationJoint->transform);

		//transform.r[3]-= transformation.r[3];
		transform = transformation * transform;
		XMStoreFloat4x4(&animationJoint->transform, transform);
		// It should be possible to do this better?  I mean there is do while but... is that really better?
		if(animationJoint->child_count > 0) {

			for(int i = 0; i < animationJoint->child_count; ++i) {

				ManipulateChildrendJoints(&tweens[animationJoint->child_index[i]], transformation, transform);
			}
			break;
		}
		else {
			break;
		}
	}

}

void Animator::ManipulateChildrendJoints(animJoint* animationJoint, const DirectX::XMMATRIX &_transformation, const XMMATRIX& animationJointParent) {

	XMMATRIX transformation = _transformation;
	while(true) {
		XMMATRIX transform = XMLoadFloat4x4(&animationJoint->transform);
		//XMMATRIX BA = transform;
		transform.r[3] -= animationJointParent.r[3];
		transform = transform * transformation;
		//transform = transformation * transform;
		transform.r[3] += animationJointParent.r[3];
		XMStoreFloat4x4(&animationJoint->transform, transform);
		// It should be possible to do this better?  I mean there is do while but... is that really better?
		if(animationJoint->child_count > 0) {

			for(int i = 0; i < animationJoint->child_count; ++i) {

				ManipulateChildrendJoints(&tweens[animationJoint->child_index[i]], transformation, transform);
			}
			break;
		}
		else {
			break;
		}
	}

}

void Animator::Destroy() {
	EngineStructure::Update.Remove(updateID);
}

void Animator::Initialize(AnimationManager* animManIn) {
	animMan = animManIn;
	updateID = EngineStructure::Update.Add([=]() {
		this->Update();
	});
}

void Animator::Update() {

	timePos += GhostTime::DeltaTime();
	bool loopState = false;
	if(timePos < 0.0) {
		timePos = currAnim->keyframes[currAnim->keyframes.size() - 1].endTime;
	}
	if(timePos > currAnim->keyframes[currAnim->keyframes.size() - 1].endTime) {
		timePos = 0.0;
	}

	keyframe beginFrame;
	keyframe endFrame;

	if(timePos < currAnim->keyframes[0].endTime) {
		beginFrame = currAnim->keyframes[currAnim->keyframes.size() - 1];
		endFrame = currAnim->keyframes[0];
		loopState = true;
	}
	else {
		for(size_t i = 0; i < currAnim->keyframes.size() - 1; ++i) {
			if(timePos > currAnim->keyframes[i].endTime && timePos <= currAnim->keyframes[i + 1].endTime) {
				beginFrame = currAnim->keyframes[i];
				endFrame = currAnim->keyframes[i + 1];
			}
		}
	}

	float ratio;
	if(!loopState)
		ratio = (float)((timePos - beginFrame.endTime) / (endFrame.endTime - beginFrame.endTime));
	else
		ratio = (float)(timePos / endFrame.endTime);

	for(size_t i = 0; i < tweens.size(); ++i) {
		DirectX::XMFLOAT3X3 endJointMat = pullRotation(endFrame.joints[i].transform);
		DirectX::XMFLOAT3X3 beginJointMat = pullRotation(beginFrame.joints[i].transform);
		DirectX::XMFLOAT3X3 interpolatedMat = lerpRotation(beginJointMat, endJointMat, ratio);
		tweens[i].transform._41 = beginFrame.joints[i].transform._41 + ((endFrame.joints[i].transform._41 - beginFrame.joints[i].transform._41)*ratio);
		tweens[i].transform._42 = beginFrame.joints[i].transform._42 + ((endFrame.joints[i].transform._42 - beginFrame.joints[i].transform._42)*ratio);
		tweens[i].transform._43 = beginFrame.joints[i].transform._43 + ((endFrame.joints[i].transform._43 - beginFrame.joints[i].transform._43)*ratio);
		tweens[i].transform._11 = interpolatedMat._11;
		tweens[i].transform._12 = interpolatedMat._12;
		tweens[i].transform._13 = interpolatedMat._13;
		tweens[i].transform._21 = interpolatedMat._21;
		tweens[i].transform._22 = interpolatedMat._22;
		tweens[i].transform._23 = interpolatedMat._23;
		tweens[i].transform._31 = interpolatedMat._31;
		tweens[i].transform._32 = interpolatedMat._32;
		tweens[i].transform._33 = interpolatedMat._33;
		for(int x = 0; x < tweens[i].child_count; ++x) {

			//DebugRenderer::AddLine((XMFLOAT3)tweens[tweens[i].child_index[x]].transform.m[3], (XMFLOAT3)tweens[i].transform.m[3], { 0.1f, 1, 0 });
		}

	}
}


void Animator::SetTime(float _timePos) {
	timePos = _timePos;
	bool loopState = false;
	if(timePos < 0.0) {
		timePos = currAnim->keyframes[currAnim->keyframes.size() - 1].endTime;
	}
	if(timePos > currAnim->keyframes[currAnim->keyframes.size() - 1].endTime) {
		timePos = 0.0;
	}

	keyframe beginFrame;
	keyframe endFrame;

	if(timePos < currAnim->keyframes[0].endTime) {
		beginFrame = currAnim->keyframes[currAnim->keyframes.size() - 1];
		endFrame = currAnim->keyframes[0];
		loopState = true;
	}
	else {
		for(size_t i = 0; i < currAnim->keyframes.size() - 1; ++i) {
			if(timePos > currAnim->keyframes[i].endTime && timePos <= currAnim->keyframes[i + 1].endTime) {
				beginFrame = currAnim->keyframes[i];
				endFrame = currAnim->keyframes[i + 1];
			}
		}
	}

	float ratio;
	if(!loopState)
		ratio = (float)((timePos - beginFrame.endTime) / (endFrame.endTime - beginFrame.endTime));
	else
		ratio = (float)(timePos / endFrame.endTime);

	for(size_t i = 0; i < tweens.size(); ++i) {
		DirectX::XMFLOAT3X3 endJointMat = pullRotation(endFrame.joints[i].transform);
		DirectX::XMFLOAT3X3 beginJointMat = pullRotation(beginFrame.joints[i].transform);
		DirectX::XMFLOAT3X3 interpolatedMat = lerpRotation(beginJointMat, endJointMat, ratio);
		tweens[i].transform._41 = beginFrame.joints[i].transform._41 + ((endFrame.joints[i].transform._41 - beginFrame.joints[i].transform._41)*ratio);
		tweens[i].transform._42 = beginFrame.joints[i].transform._42 + ((endFrame.joints[i].transform._42 - beginFrame.joints[i].transform._42)*ratio);
		tweens[i].transform._43 = beginFrame.joints[i].transform._43 + ((endFrame.joints[i].transform._43 - beginFrame.joints[i].transform._43)*ratio);
		tweens[i].transform._11 = interpolatedMat._11;
		tweens[i].transform._12 = interpolatedMat._12;
		tweens[i].transform._13 = interpolatedMat._13;
		tweens[i].transform._21 = interpolatedMat._21;
		tweens[i].transform._22 = interpolatedMat._22;
		tweens[i].transform._23 = interpolatedMat._23;
		tweens[i].transform._31 = interpolatedMat._31;
		tweens[i].transform._32 = interpolatedMat._32;
		tweens[i].transform._33 = interpolatedMat._33;
	}
}

void Animator::addAnim(const char * animFilePath, const char * bindposeFilePath, const char * animName) {
	animations[std::string(animName)] = animMan->GetReferenceAnimation(animFilePath, bindposeFilePath);
	if(!currAnim) {
		currAnim = animations[std::string(animName)];
		tweens = animations[std::string(animName)]->keyframes[0].joints;
		for(size_t i = 0; i < tweens.size(); ++i) {
			jointPointers[tweens[i].name] = (int)i;
		}
	}
}

bool Animator::setState(const char * animName) {
	Animation* toSet = animations[std::string(animName)];
	if(toSet) {
		currAnim = toSet;
		return true;
	}
	return false;
}
