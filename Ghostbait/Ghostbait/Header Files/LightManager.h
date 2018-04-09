#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#define MAX_LIGHTS 83

struct genericLight {
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 dir;
	float radius = 0.0f;
	DirectX::XMFLOAT3 pos;
	float outerRadius = 0.0f;
};

struct lightBufferStruct {
	DirectX::XMFLOAT3 ambientColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambientIntensity = 0.3f;
	DirectX::XMFLOAT3 cameraPos;
	float padding;
};

class LightManager
{
	static int numLights;
	static int nextID;
	static lightBufferStruct cpu_light_info;
	static int IDList[MAX_LIGHTS];
	static ID3D11Buffer* lightBuffer;
	static ID3D11ShaderResourceView* srv;
	static genericLight cpu_side_lights[MAX_LIGHTS];
public:
	static int addLight(genericLight toAdd);
	static void setAmbient(DirectX::XMFLOAT3 color, float factor);
	static lightBufferStruct* getLightBuffer() { return &cpu_light_info; };
	static genericLight* getLight(int ID);
	static void removeLight(int ID);
	static void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	static void BindLightArray(ID3D11DeviceContext* context);
	static void Destroy();
	
	static int addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir);
	static int addPointLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius);
	static int addSpotLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius);

	LightManager();
	~LightManager();
};