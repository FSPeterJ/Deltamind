#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>
#include "Font.h"
#include <string>
#include <vector>
#include "Material.h"

struct VertexPositionTexture;

class TextManager {
	struct renderableMat
	{
		Material* mat;
		ID3D11RenderTargetView* rtv;
		ID3D11DepthStencilView* dsv;
		ID3D11Texture2D* depthTex;
		D3D11_VIEWPORT viewport;
		float width;
		float height;
	};

	struct windowSizeBuffer
	{
		float height;
		float width;
		float fill1;
		float fill2; //DirectX is a HOOT
	};

	DirectX::XMFLOAT4 textColor;
		

	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11VertexShader* vs;
	static ID3D11PixelShader* ps;
	static ID3D11InputLayout* il;
	static windowSizeBuffer sizeBuffer;
	static DirectX::XMFLOAT4 ColorBuffer;
	static ID3D11Buffer* windowSizeToShader;
	static ID3D11Buffer* colorToShader;
	static std::unordered_map<std::string, Font*> fonts;
	static std::vector<renderableMat> managedMaterials;

	static renderableMat createTextMaterial(float width, float height);
	static void remakeMaterial(renderableMat* toMake, float width, float height);
	static renderableMat generateVertexBufferAndRender(Font* font, std::string _sentence, DirectX::XMFLOAT4 &bgColor, int index = -1);
	static void releaseRenderableMat(renderableMat* _mat);
	static void renderText(renderableMat* mat, std::string& sentence, std::vector<VertexPositionTexture> & vertices, ID3D11ShaderResourceView * font, DirectX::XMFLOAT4 &bgColor);
public:
	struct textOutput
	{
		Material* mat;
		float heightOverWidthRatio;
	};
	static void Initialize(ID3D11Device* _device, ID3D11DeviceContext* _context, ID3D11VertexShader* _vs, ID3D11PixelShader* _ps, ID3D11InputLayout* _il);
	static void Destroy();

	static void LoadFont(std::string _fileName, std::string _texturePath);
	static Material* CreateRenderableTexture(float width, float height);
	static textOutput DrawTextTo(std::string _fontTexturePath, std::string _sentence, DirectX::XMFLOAT4 textColor, DirectX::XMFLOAT4 backgroundColor);
	static float DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material* _mat, DirectX::XMFLOAT4 textColor, DirectX::XMFLOAT4 backgroundColor); //Returns a suggested aspect ratio for the quad as Y = (X * return)
};