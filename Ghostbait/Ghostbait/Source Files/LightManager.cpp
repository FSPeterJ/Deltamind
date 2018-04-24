#include "LightManager.h"

int LightManager::numLights = 0;
int LightManager::nextID = 0;
lightBufferStruct LightManager::cpu_light_info;
int LightManager::IDList[MAX_LIGHTS];
genericLight LightManager::cpu_side_lights[MAX_LIGHTS];
ID3D11Buffer* LightManager::toShader;

int LightManager::addLight(genericLight toAdd)
{
	if (numLights < MAX_LIGHTS) {
		cpu_side_lights[numLights] = toAdd;
		IDList[numLights] = nextID;
		numLights++;
		return nextID++;
	}
	return -1; //Can't fit any more
}

void LightManager::setAmbient(DirectX::XMFLOAT3 color, float factor)
{
	cpu_light_info.ambientIntensity = factor;
	cpu_light_info.ambientColor = color;
}

genericLight * LightManager::getLight(int ID)
{
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (cpu_side_lights[i].color.w == 0.0f)
			break;
		if (IDList[i] == ID)
		{
			return cpu_side_lights[i].color.w == 0.0f ? nullptr : &cpu_side_lights[i];
		}
	}
	return nullptr;
}

void LightManager::removeLight(int ID)
{
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (cpu_side_lights[i].color.w == 0.0f)
			break;
		if (IDList[i] == ID)
		{
			if (i == numLights - 1)
			{
				cpu_side_lights[--numLights].color.w = 0.0f;
				break;
			}
			genericLight temp = cpu_side_lights[numLights - 1];
			cpu_side_lights[numLights - 1].color.w = 0.0f;
			cpu_side_lights[i] = temp;
			IDList[i] = IDList[numLights - 1];
			IDList[--numLights] = ID;
			break;
		}
	}
}

void LightManager::bindToShader(ID3D11DeviceContext * context)
{
	context->UpdateSubresource(toShader, NULL, NULL, &cpu_side_lights, NULL, NULL);
	context->PSSetConstantBuffers(4, 1, &toShader);
}

int LightManager::addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.dir = dir;
	return addLight(toManager);
}

int LightManager::addPointLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.pos = pos;
	toManager.radius = radius;
	return addLight(toManager);
}

int LightManager::addSpotLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.pos = pos;
	toManager.dir = dir;
	toManager.radius = radius;
	toManager.outerRadius = outerRadius;
	return addLight(toManager);
}

void LightManager::Initialize(ID3D11Device * device)
{
	CD3D11_BUFFER_DESC temp = CD3D11_BUFFER_DESC(sizeof(genericLight)*MAX_LIGHTS, D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&temp, NULL, &toShader);
}

void LightManager::Release()
{
	toShader->Release();
}

LightManager::LightManager()
{
}


LightManager::~LightManager()
{
}