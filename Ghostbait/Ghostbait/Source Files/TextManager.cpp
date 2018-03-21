#include "TextManager.h"
#include "VertexTypes.h"

ID3D11Device* TextManager::device;
ID3D11DeviceContext* TextManager::context;
std::unordered_map<std::string, Font*> TextManager::fonts; 
std::vector<TextManager::renderableMat> TextManager::managedMaterials;

TextManager::renderableMat TextManager::createTextMaterial(float width, float height)
{
	renderableMat toPush;
	toPush.mat = new Material();
	toPush.viewport = D3D11_VIEWPORT();
	toPush.viewport.Height = height;
	toPush.viewport.Width = width;
	toPush.viewport.MaxDepth = 1.0f;
	toPush.viewport.MinDepth = 0.0f;
	toPush.viewport.TopLeftX = 0.0f;
	toPush.viewport.TopLeftY = 0.0f;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Height = (UINT)height;
	texDesc.Width = (UINT)width;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc = sampleDesc;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture2D.MostDetailedMip = 0;

	device->CreateTexture2D(&texDesc, nullptr, (ID3D11Texture2D**)&toPush.mat->diffuse.texture);
	device->CreateShaderResourceView(toPush.mat->diffuse.texture, &viewDesc, &toPush.mat->diffuse.texView);
	toPush.mat->diffuse.factor = 1.0f;
	toPush.mat->emissive.factor = 0.0f;
	toPush.mat->specular.factor = 0.0f;

	device->CreateRenderTargetView(toPush.mat->diffuse.texture, nullptr, &toPush.rtv);

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;

	device->CreateTexture2D(&texDesc, nullptr, &toPush.depthTex);
	device->CreateDepthStencilView(toPush.depthTex, &depthStencilDesc, &toPush.dsv);
	managedMaterials.push_back(toPush);
	return toPush;
}

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
		managedMaterials[i].depthTex->Release();
	}
}

void TextManager::LoadFont(std::string _fileName, std::string _texturePath)
{
	fonts[_texturePath] = new Font(_fileName, _texturePath, device, context);
}

TextManager::textOutput TextManager::DrawTextTo(std::string _fontTexturePath, std::string _sentence)
{
	Font* font = fonts[_fontTexturePath];
	if (!font)
		return textOutput();
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
