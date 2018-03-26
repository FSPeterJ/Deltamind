#include "TextManager.h"
#include "VertexTypes.h"

ID3D11Device* TextManager::device;
ID3D11DeviceContext* TextManager::context;
ID3D11VertexShader* TextManager::vs;
ID3D11PixelShader* TextManager::ps;
ID3D11InputLayout* TextManager::il;
TextManager::windowSizeBuffer TextManager::sizeBuffer;
DirectX::XMFLOAT4 TextManager::ColorBuffer;
ID3D11Buffer* TextManager::windowSizeToShader;
std::unordered_map<std::string, Font*> TextManager::fonts; 
std::vector<TextManager::renderableMat> TextManager::managedMaterials;
ID3D11Buffer* TextManager::colorToShader;

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
	toPush.mat->flags = Material::MaterialFlags::POINT;
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
	toPush.width = width;
	toPush.height = height;
	managedMaterials.push_back(toPush);
	return toPush;
}

TextManager::renderableMat TextManager::generateVertexBufferAndRender(Font * font, std::string _sentence, DirectX::XMFLOAT4 &bgColor, int index)
{
	bool preMade = index != -1;
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
		if (_sentence[i] == ' ')
		{
			tempWidth += 3.0f;
			continue;
		}
		CharPos pos = font->GetCharPos(_sentence[i]);
		tempWidth += pos.size + 1.0f;
		if (tempWidth > width)
			width = tempWidth;
	}
	float widRatio = 1.0f;
	float heightRatio = 1.0f;

	if (preMade)
	{
		widRatio = managedMaterials[index].width / width;
		heightRatio = managedMaterials[index].height / height;
	}


	float heightOverWidthRatio = height / width; //Used to properly scale quads

	float drawX = 0.0f;
	float drawY = 0.0f;
	std::vector<VertexPositionTexture> vertices;
	for (size_t i = 0; i < _sentence.length(); ++i)
	{
		if (_sentence[i] == ' ')
		{
			drawX += 3.0f * widRatio;
		}
		else if (_sentence[i] == '\n')
		{
			drawY += 16.0f * heightRatio;
			drawX = 0.0f;
		}
		else
		{
			CharPos pos = font->GetCharPos(_sentence[i]);
			VertexPositionTexture toPush;
			toPush.pos = DirectX::XMFLOAT3(drawX, drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + (pos.size * widRatio), drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX, drawY + (16.0f*heightRatio), 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 1.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + (pos.size * widRatio), drawY, 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 0.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX + (pos.size * widRatio), drawY + (16.0f*heightRatio), 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.endU, 1.0f);
			vertices.push_back(toPush);

			toPush.pos = DirectX::XMFLOAT3(drawX, drawY + (16.0f * heightRatio), 0.0f);
			toPush.tex = DirectX::XMFLOAT2(pos.startU, 1.0f);
			vertices.push_back(toPush);

			drawX = drawX + ((pos.size * widRatio) + 1.0f);
		}
	}
	renderableMat mat;

	if (preMade)
	{
		sizeBuffer.height = managedMaterials[index].height;
		sizeBuffer.width = managedMaterials[index].width;
		mat = managedMaterials[index];
	}
	else
	{
		sizeBuffer.height = height;
		sizeBuffer.width = width;
		mat = createTextMaterial(width, height);
	}
	ID3D11ShaderResourceView * srv = font->GetShaderResourceView();
	renderText(&mat, _sentence, vertices, srv, bgColor);
	
	mat.height = height;
	mat.width = width;
	return mat;
}

void TextManager::renderText(renderableMat * mat, std::string & sentence, std::vector<VertexPositionTexture>& vertices, ID3D11ShaderResourceView * font, DirectX::XMFLOAT4 & bgColor)
{
	ID3D11Buffer* vertexBuffer;
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &vertices[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionTexture) * (UINT)vertices.size(), D3D11_BIND_VERTEX_BUFFER);

	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = 0;

	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
	context->UpdateSubresource(windowSizeToShader, NULL, NULL, &sizeBuffer, NULL, NULL);
	context->UpdateSubresource(colorToShader, NULL, NULL, &ColorBuffer, NULL, NULL);
	float color[] = { bgColor.x, bgColor.y, bgColor.z, bgColor.w };
	context->ClearRenderTargetView(mat->rtv, color);
	context->ClearDepthStencilView(mat->dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &mat->rtv, mat->dsv);
	context->RSSetViewports(1, &mat->viewport);

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetInputLayout(il);

	context->VSSetShader(vs, NULL, NULL);
	context->VSSetConstantBuffers(3, 1, &windowSizeToShader);

	context->PSSetShader(ps, NULL, NULL);
	context->PSSetConstantBuffers(3, 1, &colorToShader);
	context->PSSetShaderResources(0, 1, &font);

	context->Draw((UINT)vertices.size(), 0);
	vertexBuffer->Release();
}

void TextManager::Initialize(ID3D11Device * _device, ID3D11DeviceContext * _context, ID3D11VertexShader* _vs, ID3D11PixelShader* _ps, ID3D11InputLayout* _il)
{
	device = _device;
	context = _context;
	vs = _vs;
	ps = _ps;
	il = _il;
	CD3D11_BUFFER_DESC modelBufferDesc(sizeof(windowSizeBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&modelBufferDesc, nullptr, &windowSizeToShader);

	CD3D11_BUFFER_DESC colorBufferDesc(sizeof(DirectX::XMFLOAT4), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&colorBufferDesc, nullptr, &colorToShader);
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
	windowSizeToShader->Release();
	colorToShader->Release();
}

void TextManager::LoadFont(std::string _fileName, std::string _texturePath)
{
	fonts[_texturePath] = new Font(_fileName, _texturePath, device, context);
}

Material * TextManager::CreateRenderableTexture(float width, float height)
{
	renderableMat mat = createTextMaterial(width, height);
	return mat.mat;
}

TextManager::textOutput TextManager::DrawTextTo(std::string _fontTexturePath, std::string _sentence, DirectX::XMFLOAT4 textColor, DirectX::XMFLOAT4 backgroundColor)
{
	Font* font = fonts[_fontTexturePath];
	if (!font)
		return textOutput();
	ColorBuffer= textColor;
	textOutput ret;
	renderableMat mat = generateVertexBufferAndRender(font, _sentence, backgroundColor);
	
	ret.mat = mat.mat;
	ret.heightOverWidthRatio = mat.height / mat.width;
	return ret;
}

float TextManager::DrawTextExistingMat(std::string _fontTexturePath, std::string _sentence, Material * _mat, DirectX::XMFLOAT4 textColor, DirectX::XMFLOAT4 backgroundColor)
{
	Font* font = fonts[_fontTexturePath];
	if (!font)
		return -1.0f;
	for (size_t i = 0; i < managedMaterials.size(); ++i)
	{
		if (_mat == managedMaterials[i].mat)
		{
			ColorBuffer = textColor;
			renderableMat mat = generateVertexBufferAndRender(font, _sentence, backgroundColor, (int)i);
			return mat.height / mat.width;
		}
	}
	return -1.0f;
}
