#pragma once
#include "EnemyBase.h"

class TargetEnemy : public EnemyBase {
	DirectX::XMFLOAT3 startPos, endPos;
	DirectX::XMFLOAT3 s2e, e2s;
	float dist;
	bool toEnd = true;
	float speed = 1;
	

	void SwitchTarget();
	bool IsPastTarget();
public:
	void SetRange(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end);
	void SetRange(const DirectX::XMFLOAT3& delta);
	void Patrol() override;
};