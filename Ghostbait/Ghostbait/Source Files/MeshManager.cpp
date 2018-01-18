#include "MeshManager.h"
#include <fstream>


void MeshManager::generateCube()
{
#pragma region pleasedontopenthis
	//you fool
	VertexPositionColor cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-0.5f, -0.5f,  -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-0.5f, -0.5f,  -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },

	{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(0.5f,  0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(0.5f,  0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
	{ XMFLOAT3(0.5f, -0.5f,  -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

	{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f,  -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.5f,  -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f, -0.5f,  -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },

	{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(0.5f, 0.5f,  -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

	{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f,  -0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f, 0.5f,  -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(-0.5f,  -0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(0.5f, 0.5f,  -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },

	{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.5f,  -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.5f,  -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
	};

	unsigned int cubeIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };
#pragma endregion
	Mesh tempMesh;
	tempMesh.meshId = UINT_MAX;
	tempMesh.indexCount = 36;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &tempMesh.vertexBuffer);
	vertexBufferData.pSysMem = &cubeIndices[0];
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*tempMesh.indexCount, D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(&indexBufferDesc, &vertexBufferData, &tempMesh.indexBuffer);
	trackedMeshes.push_back(tempMesh);
}

MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
}

void MeshManager::Initialize(ID3D11Device* deviceIn)
{
	device = deviceIn;
	//generateCube();
	Mesh* toEdit = ConstructMesh("Assets/TestCube_mesh.bin");
	toEdit->meshId = UINT_MAX;
}

void MeshManager::Destroy()
{
	for(size_t i = 0; i < trackedMeshes.size(); ++i)
	{
		trackedMeshes[i].vertexBuffer->Release();
		trackedMeshes[i].indexBuffer->Release();
	}
}

Mesh* MeshManager::ConstructMesh(const char* _meshFilePath)
{
	Mesh loadingIn;
	std::vector<VertexPositionTextureNormalAnim> verts;
	std::vector<int> indices;
	std::ifstream reader;
	reader.open(_meshFilePath, std::ios_base::binary);
	int vCount;
	int iCount;
	if(reader.is_open())
	{
		reader.read((char*)&vCount, sizeof(vCount));
		for(int i = 0; i < vCount; ++i)
		{
			DirectX::XMFLOAT4 pos;
			//float throwitallaway; //Used when the incoming data is unused
			VertexPositionTextureNormalAnim temp;
			reader.read((char*)&pos.x, sizeof(pos.x));
			reader.read((char*)&pos.y, sizeof(pos.y));
			reader.read((char*)&pos.z, sizeof(pos.z));
			reader.read((char*)&pos.w, sizeof(pos.w));
			temp.pos = pos;
			reader.read((char*)&temp.normal.x, sizeof(temp.normal.x));
			reader.read((char*)&temp.normal.y, sizeof(temp.normal.y));
			reader.read((char*)&temp.normal.z, sizeof(temp.normal.z));
			reader.read((char*)&temp.UV.x, sizeof(temp.UV.x));
			reader.read((char*)&temp.UV.y, sizeof(temp.UV.y));

			for(int infIndex = 0; infIndex < 4; ++infIndex)
			{
				reader.read((char*)&temp.indices[infIndex], sizeof(int));
				reader.read((char*)&temp.weights[infIndex], sizeof(float));
			}
			verts.push_back(temp);
		}
		reader.read((char*)&iCount, sizeof(iCount));
		for(int i = 0; i < iCount; ++i)
		{
			int index;
			reader.read((char*)&index, sizeof(index));
			indices.push_back(index);
		}
	}
	reader.close();

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &verts[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionTextureNormalAnim) * (UINT)verts.size(), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &loadingIn.vertexBuffer);
	vertexBufferData.pSysMem = &indices[0];
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(int)*(UINT)indices.size(), D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(&indexBufferDesc, &vertexBufferData, &loadingIn.indexBuffer);
	loadingIn.indexCount = (unsigned int)indices.size();
	trackedMeshes.push_back(loadingIn);
	loadingIn.meshId = (unsigned int)trackedMeshes.size();
	return &trackedMeshes[trackedMeshes.size() - 1];
}


int MeshManager::AddElement(const char* _meshFilePath)
{
	Mesh* mesh = ConstructMesh(_meshFilePath);

	return mesh->meshId;
}


Mesh*  MeshManager::GetElement(const unsigned int _id)
{
	//
	for(size_t i = 0; i < trackedMeshes.size(); ++i)
	{
		if(trackedMeshes[i].meshId == _id)
			return &trackedMeshes[i];
	}
	return nullptr;
}
