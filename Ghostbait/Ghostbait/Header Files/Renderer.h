#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Window.h"
#include <vector>

class Transform;
class GameObject;
class Object;
class VRManager;
class MeshManager;
class MaterialManager;
class AnimationManager;
class EventMessageBase;
class LightManager;
struct Mesh;

enum renderState {
	RENDER_STATE_DEFAULT, RENDER_STATE_TRANSPARENT
};


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
		DirectX::XMFLOAT3 camPos;
	};

	struct animDataBufferStruct {
		DirectX::XMFLOAT4X4 cpu_side_joints[50];
		bool willAnimate;
		DirectX::XMFLOAT3 filler;
	};

	struct Skybox
	{
		ID3D11Texture2D* faces[6];
		ID3D11Texture2D* box;
		ID3D11ShaderResourceView* srv;
	};

	struct DeferredRTVs
	{
		ID3D11Texture2D* textures[4];
		ID3D11Texture2D* depthBuffer;
		ID3D11RenderTargetView* RTVs[4];
		ID3D11DepthStencilView* DSV;
		ID3D11ShaderResourceView* SRVs[4];
	};
#pragma endregion

	ID3D11SamplerState* OnlySamplerState; //DirectX is a hoot

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11Texture2D* backBuffer;
	DeferredRTVs deferredTextures;

	ID3D11VertexShader* PassThroughPositionColorVS;
	ID3D11PixelShader* PassThroughPS;
	ID3D11VertexShader* StandardVertexShader;
	ID3D11PixelShader* StandardPixelShader;
	ID3D11VertexShader* ParticleVS;
	ID3D11GeometryShader* ParticleGS;
	ID3D11PixelShader* ParticlePS;
	ID3D11VertexShader* SkyboxVS;
	ID3D11PixelShader* SkyboxPS;
	ID3D11PixelShader* DeferredTargetPS;

	ID3D11InputLayout* ILPositionColor;
	ID3D11InputLayout* ILStandard;
	ID3D11InputLayout* ILParticle;

	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* modelBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* animDataBuffer;
	pipeline_state_t defaultPipeline;
	Transform* cameraPos;
	viewProjectionConstantBuffer defaultCamera;
	animDataBufferStruct cpuAnimationData;
	Mesh* skyball;

	Skybox* currSkybox = nullptr;

	std::vector<const GameObject*> renderedObjects;
	std::vector<const GameObject*> frontRenderedObjects;

	MeshManager* meshManagement = nullptr;
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
	void drawSkyboxTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport, DirectX::XMFLOAT3& pos);
	void loadPipelineState(pipeline_state_t* pipeline);
	void createDeferredRTVs();
	void createRTVandSRV(ID3D11Texture2D** texture, ID3D11ShaderResourceView** srv, ID3D11RenderTargetView** rtv);

	DirectX::XMFLOAT4X4 lookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);

	void loadSkyboxFaces(Skybox* toLoad, const char* directory, const char* filePrefix);

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
	void Initialize(Window window, Transform* _cameraPos);

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

	void moveToFront(EventMessageBase* e);
	//////////////////////////////////////////////////////////////////////////////////
	void setSkybox(const char* directoryName, const char* filePrefix);

	MeshManager* getMeshManager();
	MaterialManager* getMaterialManager();
	AnimationManager* getAnimationManager();
	Transform* getCamera();
	void Render();
};
