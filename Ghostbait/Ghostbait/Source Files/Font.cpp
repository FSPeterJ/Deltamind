#include "Font.h"
#include <fstream>
#include "WICTextureLoader.h"
#include <d3d11.h>

CharPos::CharPos() {
	startU = 0;
	endU = 0;
	size = 0;
}
CharPos::CharPos(float _startX, float _endX, float _size) {
	startU = _startX;
	endU = _endX;
	size = _size;
}


bool Font::LoadIndexFile(std::string _fileName, float width)
{
	std::ifstream reader;
	reader.open(_fileName);
	if (!reader.is_open())
		return false;

	char readTo;
	float currPlace = -1.0f;
	for (int i = 0; i < 95; ++i)
	{
		reader.get(readTo);
		while (readTo != ' ')
			reader.get(readTo);
		reader.get(readTo);
		char letter = readTo;
		CharPos temp;
		reader >> temp.startU;
		reader >> temp.endU;
		reader >> temp.size;
		temp.startU = currPlace / width;
		temp.endU = (currPlace + temp.size) / width;
		currPlace += temp.size + 1.0f;
		charMap[letter] = temp;
	}
	reader.close();
	return true;
}

bool Font::LoadTexture(std::string _fileName, ID3D11Device* device, ID3D11DeviceContext* context)
{
	std::wstring path(_fileName.begin(), _fileName.end());
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&tex, &srv);
	if (didItBlend == S_OK)
		return true;
	return false;
}

Font::Font() {

}
Font::Font(std::string _fileName, std::string _texturePath, ID3D11Device* device, ID3D11DeviceContext* context) {
	if (LoadTexture(_texturePath, device, context))
	{
		D3D11_TEXTURE2D_DESC desc;
		tex->GetDesc(&desc);
		LoadIndexFile(_fileName, (float)desc.Width);
	}
}

Font::~Font()
{
	tex->Release();
	srv->Release();
}

const CharPos Font::GetCharPos(char c) {
	if (charMap.find(c) != charMap.end())
		return charMap[c];
	else
		return InvalidCharPos();
}

float Font::GetTextureHeight()
{
	if (!tex)
		return -1.0f;

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);
	return (float)desc.Height;
}
