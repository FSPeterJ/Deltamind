#include "Light.h"
#include "LightManager.h"

void Light::RemoveLightFromManager() {
	if (LightManager::getLight(lightID)) {
		LightManager::removeLight(lightID);
	}
}

void Light::SetAsDirectional(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir) {
	RemoveLightFromManager();
	lightID = LightManager::addDirectionalLight(color, dir);
	type = LIGHT_Directional;
}
void Light::SetAsPoint(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius) {
	RemoveLightFromManager();
	lightID = LightManager::addPointLight(color, pos, radius);
	type = LIGHT_Point;
}
void Light::SetAsSpot(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius) {
	RemoveLightFromManager();
	lightID = LightManager::addSpotLight(color, pos, dir, radius, outerRadius);
	type = LIGHT_Spot;
}

void Light::SetColor(DirectX::XMFLOAT3 color) {
	genericLight* data = GetData();
	if (data) data->color = { color.x, color.y, color.z, 1 };
}
void Light::SetColor(DirectX::XMFLOAT4 color) {
	genericLight* data = GetData();
	if (data) data->color = color;
}
void Light::SetRadius(const float radius) {
	genericLight* data = GetData();
	if (data && (type == LIGHT_Point || type == LIGHT_Spot)) data->radius = radius;
}
void Light::SetDirection(const DirectX::XMFLOAT3 dir) {
	genericLight* data = GetData();
	if (data && (type == LIGHT_Directional || type == LIGHT_Spot)) data->dir = dir;
}
void Light::SetOuterRadius(const float outerRadius) {
	genericLight* data = GetData();
	if (data && type == LIGHT_Spot) data->outerRadius = outerRadius;
}

genericLight* Light::GetData() const {
	return LightManager::getLight(lightID);
}

void Light::Update() {
	LightManager::getLight(lightID)->pos = transform.GetPosition();
	GameObject::Update();
}