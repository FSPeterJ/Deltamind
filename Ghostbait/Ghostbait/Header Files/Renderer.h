#pragma once

#include <d3d11.h>
#include "MeshManager.h"
#include "Window.h"
#include "VRManager.h"
#include "Object.h"
#include "MessageEvents.h"
#include "MaterialManager.h"

enum renderState
{
	RENDER_STATE_DEFAULT, RENDER_STATE_TRANSPARENT
};

struct directionalLight
{
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT3 lightDir;
	float ambient;
};
class Renderer
{
private:

#pragma region structures
	struct pipeline_state_t
	{
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

	struct viewProjectionConstantBuffer
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct renderTargetInfo
	{
		ID3D11RenderTargetView* rtv;
		ID3D11Texture2D* texture;
		ID3D11DepthStencilView* dsv;
		ID3D11Texture2D* depthBuffer;
		D3D11_VIEWPORT viewport;
	};

	struct eye
	{
		renderTargetInfo renderInfo;
		viewProjectionConstantBuffer camera;
	};

#pragma endregion

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
	ID3D11Buffer* dirLightBuffer;
	pipeline_state_t defaultPipeline;
	int tempId;
	int tempMatId;
	viewProjectionConstantBuffer defaultCamera;

	//eye leftEye;
	//eye rightEye;

	std::vector<const Object*> renderedObjects;
	
	MeshManager* meshManagement = nullptr;
	VRManager* VRManagement = nullptr;
	MaterialManager* materialManagement = nullptr;

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
	//Used to have the rendering system begin to render the position.  Only needs to be called once per position.
	//
	//Parameters: toRegister, specialInstructions
	//[toRegister] A pointer to the position to place in the system
	//[specialInstructions] Used to specify any special rendering instructions, such as transparent items.
	//////////////////////////////////////////////////////////////////////////////////
	void registerObject(const Object* toRegister, renderState specialInstructions = RENDER_STATE_DEFAULT);
	
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
	//unregisterObject
	//Used to have the rendering system stop rendering the position.  Only needs to be called once per position.
	//
	//Parameters: toRemove, specialInstructions
	//[toRegister] A pointer to the position to remove from the system
	//[specialInstructions] Used to specify any special rendering instructions, such as transparent items.
	//
	//Return value: bool
	//Used to indicate if the position was found.
	//////////////////////////////////////////////////////////////////////////////////
	bool unregisterObject(const Object* toRemove, renderState specialInstructions = RENDER_STATE_DEFAULT);
	MeshManager* getMeshManager() { return meshManagement; }
	MaterialManager* getMaterialManager() { return materialManagement; }

	void Render();
};

