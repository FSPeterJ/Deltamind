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
class HUD;
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
		ID3D11BlendState* blend_state;
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

	struct DeferredRTVs
	{
		ID3D11Texture2D* textures[6];
		ID3D11Texture2D* depthBuffer;
		ID3D11RenderTargetView* RTVs[6];
		ID3D11DepthStencilView* DSV;
		ID3D11ShaderResourceView* SRVs[6];
	};

	struct eye {
		renderTargetInfo renderInfo;
		DeferredRTVs targets;
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

	struct blurData
	{
		DirectX::XMFLOAT2 dir;
		float width;
		float height;
	};

	struct uvOffsetData
	{
		DirectX::XMFLOAT2 offsets = DirectX::XMFLOAT2(0.0f, 0.0f);
		DirectX::XMFLOAT2 padding;
	};
#pragma endregion

	ID3D11SamplerState* LinearSamplerState;
	ID3D11SamplerState* PointSamplerState; //DirectX is not a hoot

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11Texture2D* backBuffer;
	ID3D11ShaderResourceView* rightSRV = nullptr; //Used to avoid a third render pass for the monitor in VR
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
	ID3D11VertexShader* PassThroughPositionVS;
	ID3D11GeometryShader* NDCQuadGS;
	ID3D11VertexShader* TextVertexShader;
	ID3D11PixelShader* PositionTexturePixelShader;
	ID3D11PixelShader* BlurPixelShader;
	ID3D11PixelShader* TexToQuadPS;

	ID3D11InputLayout* ILPositionColor;
	ID3D11InputLayout* ILPositionTexture;
	ID3D11InputLayout* ILStandard;
	ID3D11InputLayout* ILParticle;
	ID3D11InputLayout* ILPosition;

	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* modelBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* animDataBuffer;
	ID3D11Buffer* blurDataBuffer;
	ID3D11Buffer* uvDataBuffer;

	pipeline_state_t defaultPipeline;
	Transform* cameraPos;
	viewProjectionConstantBuffer defaultCamera;
	animDataBufferStruct cpuAnimationData;
	uvOffsetData uvData;
	Mesh* skyball;
	HUD* defaultHUD;

	ID3D11Buffer* emptyFloat3Buffer; //Needed to upload to the shaders that don't need specific vertex values (may replace with techniques later)
	Skybox* currSkybox = nullptr;

	std::vector<const GameObject*> renderedObjects;
	std::vector<const GameObject*> frontRenderedObjects;
	std::vector<const GameObject*> transparentObjects;

	MeshManager* meshManagement = nullptr;
	MaterialManager* materialManagement = nullptr;
	AnimationManager* animationManagement = nullptr;

	void initDepthStencilBuffer(pipeline_state_t* pipelineTo);
	void initDepthStencilState(pipeline_state_t* pipelineTo);
	void initDepthStencilView(pipeline_state_t* pipelineTo);
	void initBlendState(pipeline_state_t* pipelineTo);
	void initRasterState(pipeline_state_t* pipelineTo, bool wireFrame = false);
	void initShaders();
	void initViewport(RECT window, pipeline_state_t* pipelineTo);
	void createDeviceContextAndSwapchain(Window window);
	void clearPipelineMemory(pipeline_state_t* pipeline);
	void clearTextureMemory(renderTargetInfo* info);
	bool LoadShaderFromCSO(char ** szByteCode, size_t& szByteCodeSize, const char* szFileName);
	void setupVRTargets();
	void renderRightEyeToMonitor();
	void releaseDeferredTarget(DeferredRTVs* in);
	void combineDeferredTargets(DeferredRTVs* in, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport);
	bool compareDistToCam(const DirectX::XMFLOAT3& t1, const DirectX::XMFLOAT3& t2, const DirectX::XMFLOAT3& camPos);
	float manhat(const DirectX::XMFLOAT3& center1, const DirectX::XMFLOAT3& center2);

	void blurTexture(D3D11_VIEWPORT& viewport, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv, unsigned int passes, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsvIn);
	void sortTransparentObjects(DirectX::XMFLOAT3 &camPos);
	void renderObjectDefaultState(const GameObject* obj);
	void renderToEye(eye* eyeTo);
	void drawSkyboxTo(ID3D11RenderTargetView** rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport, DirectX::XMFLOAT3& pos);
	void loadPipelineState(pipeline_state_t* pipeline);
	void createDeferredRTVs(DeferredRTVs* toWrite, ID3D11Texture2D* refTex);
	void createRTVandSRV(ID3D11Texture2D** texture, ID3D11ShaderResourceView** srv, ID3D11RenderTargetView** rtv, ID3D11Texture2D* refTex, DXGI_FORMAT format);

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
	void moveToTransparent(EventMessageBase* e);

	void setSkybox(const char* directoryName, const char* filePrefix);

	MeshManager* getMeshManager();
	MaterialManager* getMaterialManager();
	AnimationManager* getAnimationManager();
	Transform* getCamera();
	void Render();
};
