#pragma once
#include "GameObject.h"

struct genericLight;

enum LightType {
	LIGHT_Point,
	LIGHT_Spot,
	LIGHT_Directional,
};
class Light : public GameObject {
	int lightID = -1;

	void RemoveLightFromManager();
public:
	void SetAsDirectional(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir);
	void SetAsPoint(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius);
	void SetAsSpot(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius);

	void SetColor(DirectX::XMFLOAT3 color);
	void SetColor(DirectX::XMFLOAT4 color);

	genericLight* GetData() const;

	void Update() override;
};