#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Window.h"
#include <vector>

class Camera;
class GameObject;
class Object;
class VRManager;
class MeshManager;
class MaterialManager;
class AnimationManager;
class EventMessageBase;

enum renderState {
	RENDER_STATE_DEFAULT, RENDER_STATE_TRANSPARENT
};

#define MAX_LIGHTS 83

class Renderer {
private:

#pragma region structures
	struct pipeline_state_t {
		ID3D11InputLayout* 	input_layout;
		ID3D11VertexShader* vertex_shader;
		ID3D11PixelShader* pixel_shader;
		ID3D11RenderTargetView* render_target_view;
		ID3D11Texture2D* depth_stencil_buffer;
		ID3D11DepthStencilState* depth_stencil_state;
		ID3D11DepthStencilView* depth_stencil_view;
		ID3D11RasterizerState* rasterizer_state;
		D3D11_VIEWPORT viewport;
	};

	struct viewProjectionConstantBuffer {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	struct renderTargetInfo {
		ID3D11RenderTargetView* rtv;
		ID3D11Texture2D* texture;
		ID3D11DepthStencilView* dsv;
		ID3D11Texture2D* depthBuffer;
		D3D11_VIEWPORT viewport;
	};

	struct eye {
		renderTargetInfo renderInfo;
		viewProjectionConstantBuffer camera;
	};

	struct genericLight {
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 dir;
		float radius = 0.0f;
		DirectX::XMFLOAT3 pos;
		float outerRadius = 0.0f;
	};

	struct lightBufferStruct {
		genericLight cpu_side_lights[MAX_LIGHTS];
		DirectX::XMFLOAT3 ambientColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		float ambientIntensity = 0.5f;
	};

	struct animDataBufferStruct {
		DirectX::XMFLOAT4X4 cpu_side_joints[50];
		bool willAnimate;
		DirectX::XMFLOAT3 filler;
	};
#pragma endregion

	class LightPool {
		int numLights;
		lightBufferStruct cpu_light_info;
	public:
		void addLight(genericLight toAdd) {
			if(numLights < MAX_LIGHTS) {
				cpu_light_info.cpu_side_lights[numLights] = toAdd;
				numLights++;
			}
		};
		void setAmbient(DirectX::XMFLOAT3 color, float factor) {
			cpu_light_info.ambientIntensity = factor;
			cpu_light_info.ambientColor = color;
		};
		lightBufferStruct* getLightBuffer() { return &cpu_light_info; };
	};
	ID3D11SamplerState* OnlySamplerState; //DirectX is a hoot

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11Texture2D* backBuffer;

	ID3D11VertexShader* PassThroughPositionColorVS;
	ID3D11PixelShader* PassThroughPS;
	ID3D11VertexShader* StandardVertexShader;
	ID3D11PixelShader* StandardPixelShader;

	ID3D11InputLayout* ILPositionColor;
	ID3D11InputLayout* ILStandard;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* modelBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* animDataBuffer;
	pipeline_state_t defaultPipeline;
	Camera* keyboardCamera;
	viewProjectionConstantBuffer defaultCamera;
	animDataBufferStruct cpuAnimationData;

	LightPool lightManager;
	//eye leftEye;
	//eye rightEye;

	std::vector<const GameObject*> renderedObjects;

	MeshManager* meshManagement = nullptr;
	VRManager* VRManagement = nullptr;
	MaterialManager* materialManagement = nullptr;
	AnimationManager* animationManagement = nullptr;

	void initDepthStencilBuffer(pipeline_state_t* pipelineTo);
	void initDepthStencilState(pipeline_state_t* pipelineTo);
	void initDepthStencilView(pipeline_state_t* pipelineTo);
	void initRasterState(pipeline_state_t* pipelineTo, bool wireFrame = false);
	void initShaders();
	void initViewport(RECT window, pipeline_state_t* pipelineTo);
	void createDeviceContextAndSwapchain(Window window);
	void clearPipelineMemory(pipeline_state_t* pipeline);
	void clearTextureMemory(renderTargetInfo* info);
	bool LoadShaderFromCSO(char ** szByteCode, size_t& szByteCodeSize, const char* szFileName);
	void setupVRTargets();

	void renderObjectDefaultState(Object* obj);
	void renderToEye(eye* eyeTo);

	void loadPipelineState(pipeline_state_t* pipeline);

	DirectX::XMFLOAT4X4 lookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);

public:
	//test
	eye leftEye;
	eye rightEye;

	Renderer();
	~Renderer();

	//////////////////////////////////////////////////////////////////////////////////
	//Initialize
	//Used to initialize the rendering system.  This needs to be called before any rendering is done and is not called automatically.
	//
	//Parameters: window
	//[window] Used to initialize the render texture and viewport, among other things.
	//////////////////////////////////////////////////////////////////////////////////
	void Initialize(Window window, VRManager * vr);

	//////////////////////////////////////////////////////////////////////////////////
	//Destroy
	//Used to clean up the rendering system's memory and any subsystems.  This needs to be called before the position is destroyed and is not called automatically.
	//
	//Parameters: none
	//////////////////////////////////////////////////////////////////////////////////
	void Destroy();

	//////////////////////////////////////////////////////////////////////////////////
	//registerObject
	//Used to have the rendering system start rendering the object.
	//
	//Parameters:
	//[toRegister]
	//[specialInstructions]
	//////////////////////////////////////////////////////////////////////////////////
	void registerObject(EventMessageBase* e);

	//////////////////////////////////////////////////////////////////////////////////
	//unregisterObject
	//Used to have the rendering system stop rendering the object.
	//
	//Parameters:
	//[toRegister]
	//[specialInstructions]
	//////////////////////////////////////////////////////////////////////////////////
	void unregisterObject(EventMessageBase* e);

	//////////////////////////////////////////////////////////////////////////////////
	void addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir);
	void addPointLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius);
	void addSpotLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius);
	void setAmbient(DirectX::XMFLOAT3 color, float factor);

	MeshManager* getMeshManager();
	MaterialManager* getMaterialManager();
	AnimationManager* getAnimationManager();
	Camera* getCamera();
	void Render();
};
