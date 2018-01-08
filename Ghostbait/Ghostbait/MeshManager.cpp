#include "MeshManager.h"



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
	meshPositionColor tempMesh;
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
	smallMeshes.push_back(tempMesh);
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
	generateCube();
}

void MeshManager::Destroy()
{
	for (size_t i = 0; i < smallMeshes.size(); ++i)
	{
		smallMeshes[i].vertexBuffer->Release();
		smallMeshes[i].indexBuffer->Release();
	}
}

meshPositionColor * MeshManager::findMesh(const unsigned int id)
{
	for (size_t i = 0; i < smallMeshes.size(); ++i)
	{
		if (smallMeshes[i].meshId == id)
			return &smallMeshes[i];
	}
	return nullptr;
}
