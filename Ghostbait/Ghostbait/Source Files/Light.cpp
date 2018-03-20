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
}
void Light::SetAsPoint(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius) {
	RemoveLightFromManager();
	lightID = LightManager::addPointLight(color, pos, radius);
}
void Light::SetAsSpot(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius) {
	RemoveLightFromManager();
	lightID = LightManager::addSpotLight(color, pos, dir, radius, outerRadius);
}

void Light::SetColor(DirectX::XMFLOAT3 color) {
	genericLight* data = GetData();
	if (data) data->color = { color.x, color.y, color.z, 1 };
}
void Light::SetColor(DirectX::XMFLOAT4 color) {
	genericLight* data = GetData();
	if (data) data->color = color;
}

genericLight* Light::GetData() const {
	return LightManager::getLight(lightID);
}

void Light::Update() {
	LightManager::getLight(lightID)->pos = transform.GetPosition();
	GameObject::Update();
}