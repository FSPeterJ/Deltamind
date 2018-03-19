#include "TextManager.h"
#include "VertexTypes.h"

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

void TextManager::LoadFont(std::string _fileName, std::string _texturePath)
{
	fonts[_texturePath] = new Font(_fileName, _texturePath, device, context);
}

TextManager::textOutput TextManager::DrawTextTo(std::string _fontTexturePath, std::string _sentence)
{
	Font* font = fonts[_fontTexturePath];
	float width = 0.0f;
	float height = 16.0f;
	float tempWidth = 0.0f;
	for (size_t i = 0; i < _sentence.length(); ++i) //Getting the total width and height of the texture to create an aspect ratio
	{
		if (_sentence[i] == '\n')
		{
			height += 16.0f;
			tempWidth = 0;
			continue;
		}
		CharPos pos = font->GetCharPos(_sentence[i]);
		tempWidth += pos.size;
		if (tempWidth > width)
			width = tempWidth;
	}
	
	textOutput ret;
	ret.heightOverWidthRatio = height / width; //Used to properly scale quads
	float drawX = 0.0f;
	float drawY = 0.0f;
	std::vector<VertexPositionTexture> vertices;
	for (size_t i = 0; i < _sentence.length(); ++i)
	{
		if (_sentence[i] == ' ')
		{
			drawX += 3.0f;
		}
		else if (_sentence[i] == '\n')
		{
			drawY -= 16.0f;
			drawX = 0.0f;
		}
		else
		{
			CharPos pos = font->GetCharPos(_sentence[i]);
			VertexPositionTexture toPush;
			toPush.pos = DirectX::XMFLOAT3(drawX, drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + pos.size, drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX, drawY - 16.0f, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 1.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX, drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + pos.size, drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + pos.size, drawY - 16.0f, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 1.0f);
			vertices.push_back(toPush);

			drawX = drawX + pos.size + 1.0f;
		}
	}
	return textOutput();
}

void TextManager::DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material * _mat)
{
}
