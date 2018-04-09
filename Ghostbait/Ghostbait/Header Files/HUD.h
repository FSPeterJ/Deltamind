#pragma once
#include <d3d11.h>
#include <vector>

class HUD
{
	class BaseHUDElement
	{
	public:
		virtual ~BaseHUDElement() {};

		virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight) {};
		virtual void Draw() {};
	};

	class Crosshair : public BaseHUDElement
	{
		D3D11_VIEWPORT viewport;
		ID3D11Texture2D* tex;
		ID3D11ShaderResourceView* srv;
	public:
		~Crosshair();
		void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight) override;
		void Draw() override;
	};

	std::vector<BaseHUDElement*> HUDElements;

	void ClearHUDElements();
public:
	HUD() {};
	HUD(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight);
	~HUD();
};

