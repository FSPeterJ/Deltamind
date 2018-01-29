#include "Animator.h"
#include "EngineStructure.h"
#include "GhostTime.h"

#include <d3d11.h>

DirectX::XMFLOAT3X3 Animator::pullRotation(DirectX::XMFLOAT4X4 pullFrom)
{
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

DirectX::XMFLOAT3X3 Animator::lerpRotation(DirectX::XMFLOAT3X3 m1, DirectX::XMFLOAT3X3 m2, float ratio)
{
	DirectX::XMVECTOR v1 = XMQuaternionRotationMatrix(XMLoadFloat3x3(&m1));
	DirectX::XMVECTOR v2 = XMQuaternionRotationMatrix(XMLoadFloat3x3(&m2));
	DirectX::XMVECTOR lerpResult = DirectX::XMQuaternionSlerp(v1, v2, ratio);
	DirectX::XMFLOAT3X3 result;
	XMStoreFloat3x3(&result, DirectX::XMMatrixRotationQuaternion(lerpResult));
	return result;
}

Animator::Animator()
{
}

Animator::~Animator()
{
}

void Animator::Update()
{
	timePos += GhostTime::DeltaTime();
	bool loopState = false;
	if (timePos < 0.0)
	{
		timePos = currAnim->keyframes[currAnim->keyframes.size() - 1].endTime;
	}
	if (timePos > currAnim->keyframes[currAnim->keyframes.size() - 1].endTime)
	{
		timePos = 0.0;
	}

	keyframe beginFrame;
	keyframe endFrame;

	if (timePos < currAnim->keyframes[0].endTime)
	{
		beginFrame = currAnim->keyframes[currAnim->keyframes.size() - 1];
		endFrame = currAnim->keyframes[0];
		loopState = true;
	}
	else
	{
		for (size_t i = 0; i < currAnim->keyframes.size() - 1; ++i)
		{
			if (timePos > currAnim->keyframes[i].endTime && timePos <= currAnim->keyframes[i + 1].endTime)
			{
				beginFrame = currAnim->keyframes[i];
				endFrame = currAnim->keyframes[i + 1];
			}
		}
	}

	float ratio;
	if (!loopState)
		ratio = (float)((timePos - beginFrame.endTime) / (endFrame.endTime - beginFrame.endTime));
	else
		ratio = (float)(timePos / endFrame.endTime);

	for (size_t i = 0; i < tweens.size(); ++i)
	{
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
		DirectX::XMStoreFloat4x4(&tweens[i].transform, DirectX::XMLoadFloat4x4(&tweens[i].transform) * DirectX::XMLoadFloat4x4(&currAnim->bPose->joints[i].transform));
	}
}

void Animator::addAnim(const char * animFilePath, const char * bindposeFilePath, const char * animName)
{
}
