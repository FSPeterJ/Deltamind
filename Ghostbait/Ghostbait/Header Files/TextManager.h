#include <d3d11.h>
#include <unordered_map>
#include "Font.h"
#include <string>
#include <vector>
#include "Material.h"

class TextManager {
	struct renderableMat
	{
		Material* mat;
		ID3D11RenderTargetView* rtv;
		ID3D11DepthStencilView* dsv;
		D3D11_VIEWPORT viewport;
	};
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static std::unordered_map<std::string, Font*> fonts;
	static std::vector<renderableMat> managedMaterials;
public:
	struct textOutput
	{
		Material* mat;
		float heightOverWidthRatio;
	};
	static void Initialize(ID3D11Device* _device, ID3D11DeviceContext* _context);
	static void Destroy();

	static void LoadFont(std::string _fileName, std::string _texturePath, ID3D11Device* device, ID3D11DeviceContext* context);
	static textOutput DrawTextTo(std::string _fontTexturePath, std::string _sentence);
	static void DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material* _mat);
};