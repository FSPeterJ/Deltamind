#pragma once

#include <DirectXMath.h>

struct VertexPositionColor {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

struct VertexPositionTextureNormalAnim {
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 normal;
	int indices[4] = {0, 0, 0, 0};
	float weights[4] = {0, 0, 0, 0};
};

struct VertexPositionTexture {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
};