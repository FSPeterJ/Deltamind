#include <d3d11.h>
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

	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11VertexShader* vs;
	static ID3D11PixelShader* ps;
	static ID3D11InputLayout* il;
	static windowSizeBuffer sizeBuffer;
	static ID3D11Buffer* windowSizeToShader;
	static std::unordered_map<std::string, Font*> fonts;
	static std::vector<renderableMat> managedMaterials;

	static renderableMat createTextMaterial(float width, float height);
	static renderableMat generateVertexBufferAndRender(Font* font, std::string _sentence, int index = -1);
	static void renderText(renderableMat* mat, std::string& sentence, std::vector<VertexPositionTexture> & vertices, ID3D11ShaderResourceView * font);
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
	static textOutput DrawTextTo(std::string _fontTexturePath, std::string _sentence);
	static float DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material* _mat); //Returns a suggested aspect ratio for the quad as Y = (X * return)
};