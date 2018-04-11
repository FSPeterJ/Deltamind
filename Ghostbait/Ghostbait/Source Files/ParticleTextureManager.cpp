#include "ParticleTextureManager.h"
#include "WICTextureLoader.h"
#include "Material.h"
#include <memory>
#include "Renderer.h"


ParticleTextureManager::ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context) {
	device = _device;
	context = _context;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.MipLevels = MIP_LEVELS;
	desc.ArraySize = MAX_TEXTURES;
	desc.Height = 512;
	desc.Width = 512;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &texArray);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvdesc.Texture2DArray.MostDetailedMip = desc.MipLevels - 1;
	srvdesc.Texture2DArray.MipLevels = desc.MipLevels;
	srvdesc.Texture2DArray.ArraySize = MAX_TEXTURES;
	srvdesc.Texture2DArray.FirstArraySlice = 0;
	device->CreateShaderResourceView((ID3D11Resource*)texArray, &srvdesc, &srv);

	desc.ArraySize = 1;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &stagingTex);
}


ParticleTextureManager::~ParticleTextureManager()
{
	for (size_t i = 0; i < textures.size(); ++i)
	{
		textures[i].tex->Release();
		textures[i].srv->Release();
	}
	stagingTex->Release();
	texArray->Release();
	srv->Release();
}

int ParticleTextureManager::AddTexture(const char * filePath) {
	texture toAdd;
	std::string thePath(filePath);
	thePath = "Assets\\" + thePath;
	std::wstring path(thePath.begin(), thePath.end());
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&toAdd.tex, &toAdd.srv);
	textures.push_back(toAdd);
	srvPointers.push_back(toAdd.srv);
	//context->UpdateSubresource(stagingTex, (UINT)textures.size() - 1, nullptr, toAdd.tex, NULL, NULL);

	for(UINT j = 0; j < 1; j++) {
		//This doesn't work
		context->UpdateSubresource(stagingTex, D3D11CalcSubresource(j, (UINT)textures.size() - 1, 1), nullptr, stagingTex, NULL, NULL);
	}
	context->CopySubresourceRegion(texArray, (UINT)textures.size() - 1, 0, 0, 0, stagingTex, (UINT)textures.size() - 1, nullptr);



	return (int)srvPointers.size()-1;


	////*/
	////---------------------------------------------------------------------------------------
	////	Load all textures into Texture2D array with D3DX11CreateTextureFromFile
	////---------------------------------------------------------------------------------------
	//UINT iNumOfMaterials = g_sFileNames.size();
	//
	//vector<ID3D11Texture2D*> pTexture2Ds;
	//pTexture2Ds.resize(iNumOfMaterials);;
	//
	//for (int i=0; i<iNumOfMaterials; i++)
	//{
	//	D3DX11_IMAGE_LOAD_INFO loadInfo;

	//	loadInfo.Width = resolution;		
	//	loadInfo.Height = resolution;
	//	loadInfo.Depth  = 0;
	//	loadInfo.FirstMipLevel = 0;
	//	loadInfo.MipLevels = 0;
	//	loadInfo.Usage = D3D11_USAGE_STAGING;
	//	loadInfo.BindFlags = 0;
	//	loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
	//	loadInfo.MiscFlags = 0;
	//	loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	loadInfo.Filter = D3DX11_FILTER_NONE;
	//	loadInfo.MipFilter = D3DX11_FILTER_NONE;
	//	loadInfo.pSrcInfo  = 0;

	//	hr = D3DX11CreateTextureFromFile(pd3dDevice, g_sFileNames[i].c_str(), 		
	//		//&loadInfo, 0, (ID3D11Resource**)&pTexture2Ds[i], 0);
	//		&loadInfo, 0, &pTexture2Ds[i], 0);
	//}

	////---------------------------------------------------------------------------------------
	////	Create the texture array.  Each element in the texture 
	////		array has the same format/dimensions.
	////---------------------------------------------------------------------------------------
	//D3D11_TEXTURE2D_DESC texElementDesc;
	//pTexture2Ds[0]->GetDesc(&texElementDesc);

	//D3D11_TEXTURE2D_DESC texArrayDesc;
	//texArrayDesc.Width              = texElementDesc.Width;
	//texArrayDesc.Height             = texElementDesc.Height;
	//texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	//texArrayDesc.ArraySize          = iNumOfMaterials;
	//texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texArrayDesc.SampleDesc.Count   = 1;
	//texArrayDesc.SampleDesc.Quality = 0;
	//texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	//texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	//texArrayDesc.CPUAccessFlags     = 0;
	//texArrayDesc.MiscFlags          = 0;

	//ID3D11Texture2D* texArray = 0;
	//hr = pd3dDevice->CreateTexture2D( &texArrayDesc, 0, &texArray);
	////---------------------------------------------------------------------------------------


	////---------------------------------------------------------------------------------------
	////	Copy individual texture elements into texture array.
	////---------------------------------------------------------------------------------------
	//// for each texture element...
	//ID3D11DeviceContext* pd3dImmediateContext;
	//pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	//for(UINT i = 0; i < g_sFileNames.size(); ++i)
	//{
	//	// for each mipmap level...
	//	for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
	//	{			
	//		D3D11_MAPPED_SUBRESOURCE MappedResource;
	//		hr = pd3dImmediateContext->Map( pTexture2Ds[i], j, D3D11_MAP_READ, 0, &MappedResource ) ;			

	//		pd3dImmediateContext->UpdateSubresource(texArray, D3D11CalcSubresource(j, i, texElementDesc.MipLevels), 0, MappedResource.pData, MappedResource.RowPitch, 0);			

	//		pd3dImmediateContext->Unmap(pTexture2Ds[i], j);
	//	}
	//}	
	////---------------------------------------------------------------------------------------


	////---------------------------------------------------------------------------------------
	////					Create a resource view to the texture array.
	////---------------------------------------------------------------------------------------
	//D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	//viewDesc.Format = texArrayDesc.Format;
	//viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//viewDesc.Texture2DArray.MostDetailedMip = 0;
	//viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	//viewDesc.Texture2DArray.FirstArraySlice = 0;
	//viewDesc.Texture2DArray.ArraySize = iNumOfMaterials;

	//hr = pd3dDevice->CreateShaderResourceView(texArray, &viewDesc, &g_pTextureArray);
	////---------------------------------------------------------------------------------------

	//SAFE_RELEASE(texArray);
	//for (int i=0; i<pTexture2Ds.size(); i++)
	//	SAFE_RELEASE(pTexture2Ds[i]); 

}

unsigned ParticleTextureManager::AddTexture(Material* mat) {
	/*texture toAdd;
	mat->diffuse.texture;*/
	++activatedCount;
	//textures.push_back(toAdd);
	//srvPointers.push_back(toAdd.srv);
	//context->UpdateSubresource(stagingTex, 0, nullptr, mat->diffuse.texture, NULL, NULL);
	//context->UpdateSubresource(texArray, activatedCount - 1u, nullptr, mat->diffuse.texture, NULL, NULL);
	////D3D11_MAPPED_SUBRESOURCE MappedResource;

	////context->CopySubresourceRegion(stagingTex, activatedCount - 1u, 0, 0, 0, mat->diffuse.texture, activatedCount - 1u, nullptr);
	////for(UINT j = 0; j < MIP_LEVELS; ++j) {
	////	HRESULT hresult = context->Map((ID3D11Texture2D*)mat->diffuse.texture, 0 , D3D11_MAP_READ, 0, &MappedResource);
	////	context->UpdateSubresource(texArray, D3D11CalcSubresource(j, activatedCount - 1u, MIP_LEVELS), 0, MappedResource.pData, MappedResource.RowPitch, 0);
	////	context->Unmap(mat->diffuse.texture, j);

	////}



	return (unsigned)activatedCount - 1u;
}

void ParticleTextureManager::BindToShader() {
	//context->PSSetShaderResources(0, (UINT)textures.size(), &srv);
}
