#pragma once
#include <DirectXMath.h>
class Camera
{
	//Note: This is not inversed.  Renderer still handles inverse and transpose operations.
	DirectX::XMFLOAT4X4 transform;
public:
	Camera();
	~Camera();

	void setCamera(const DirectX::XMFLOAT4X4 matrixToUse) { transform = matrixToUse; }
	DirectX::XMFLOAT4X4 getCamera() { return transform; }

	void setCameraPosition(const float x, const float y, const float z);
	void setCameraRotationDegrees(const float x, const float y, const float z);
	void setCameraRotationRadians(const float x, const float y, const float z);
	void setCameraScale(const float x, const float y, const float z);//why would you call this
};

