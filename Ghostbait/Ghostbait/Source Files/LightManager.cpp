#include "LightManager.h"



int LightManager::addLight(genericLight toAdd)
{
	if (numLights < MAX_LIGHTS) {
		cpu_light_info.cpu_side_lights[numLights] = toAdd;
		IDList[numLights] = nextID;
		numLights++;
		return nextID++;
	}
	return 0; //Can't fit any more
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
		if (cpu_light_info.cpu_side_lights[i].color.w == 0.0f)
			break;
		if (IDList[i] == ID)
			return &cpu_light_info.cpu_side_lights[i];
	}
	return nullptr;
}

LightManager::LightManager()
{
}


LightManager::~LightManager()
{
}
