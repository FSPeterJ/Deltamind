#include "TextManager.h"

ID3D11Device* TextManager::device;
ID3D11DeviceContext* TextManager::context;
std::unordered_map<std::string, Font*> TextManager::fonts; 
std::vector<TextManager::renderableMat> TextManager::managedMaterials;

void TextManager::Initialize(ID3D11Device * _device, ID3D11DeviceContext * _context)
{
	device = _device;
	context = _context;
}

void TextManager::Destroy()
{
	for (auto iter = fonts.begin(); iter != fonts.end(); ++iter)
		delete iter->second;
	for (size_t i = 0; i < managedMaterials.size(); ++i)
	{
		managedMaterials[i].mat->release();
		delete managedMaterials[i].mat;
		managedMaterials[i].rtv->Release();
		managedMaterials[i].dsv->Release();
	}
}

void TextManager::LoadFont(std::string _fileName, std::string _texturePath, ID3D11Device * device, ID3D11DeviceContext * context)
{
	fonts[_texturePath] = new Font(_fileName, _texturePath, device, context);
}

TextManager::textOutput TextManager::DrawTextTo(std::string _fontTexturePath, std::string _sentence)
{
	Font* font = fonts[_fontTexturePath];
	float width = 0.0f;
	float height = 16.0f;
	for (size_t i = 0; i < _sentence.length(); ++i)
	{
		if (_sentence[i] == '\n')
		{
			height += 16.0f;
			continue;
		}
	}
	return textOutput();
}

void TextManager::DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material * _mat)
{
}
