#pragma once
#include <d3d11.h>
#include <vector>
#include "Player.h"

class HUD
{
	class BaseHUDElement
	{
	public:
		virtual ~BaseHUDElement() {};

		virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight) {};
		virtual void Draw(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv) {};
	};

	class Crosshair : public BaseHUDElement
	{
		D3D11_VIEWPORT viewport;
		ID3D11Texture2D* tex;
		ID3D11ShaderResourceView* srv;
	public:
		~Crosshair();
		void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight) override;
		void Draw(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv) override;
	};

	class Inventory : public BaseHUDElement
	{
		std::vector<D3D11_VIEWPORT> viewports;
		std::vector<ID3D11Texture2D*> textures;
		std::vector < ID3D11ShaderResourceView*> srvs;
		const Player* player;
		ID3D11Texture2D* inactiveTex;
		ID3D11Texture2D* activeTex;
		ID3D11ShaderResourceView* inactiveSRV;
		ID3D11ShaderResourceView* activeSRV;
		ID3D11DepthStencilView* dsv;
		ID3D11BlendState* noBlend;
		ID3D11BlendState* blend;

		void loadTexture(const wchar_t* name, ID3D11Device* device, ID3D11DeviceContext* context);
		void initBlendStates(ID3D11Device* device);
		bool determineSelected(const int index);
	public:
		~Inventory();
		void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight) override;
		void Draw(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv) override;
	};

	std::vector<BaseHUDElement*> HUDElements;
	ID3D11PixelShader* TexToQuadPS;
	ID3D11GeometryShader* PointToNDCQuadGS;
	ID3D11VertexShader* PassThroughVS;
	ID3D11InputLayout* IL;
	ID3D11Buffer* throwawayVertexBuffer;
	void ClearHUDElements();
public:
	HUD() {};
	HUD(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight, ID3D11VertexShader* vs, ID3D11GeometryShader* gs, ID3D11PixelShader* ps, ID3D11InputLayout* il);
	~HUD();

	void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);
};

