#pragma once
#include <DirectXMath.h> 
#include "GameObject.h"
#include "Controlable.h"
class Camera : public GameObject, public Controlable{
	//Note: This is not inversed.  Renderer still handles inverse and transpose operations.
public:
	Camera();
	~Camera();

	void setCamera(const DirectX::XMFLOAT4X4 matrixToUse) { position = matrixToUse; }

	void setCameraPosition(const float x, const float y, const float z);
	void setCameraRotationDegrees(const float x, const float y, const float z);
	void setCameraRotationRadians(const float x, const float y, const float z);
	void setCameraScale(const float x, const float y, const float z);//why would you call this -- to adjust perspective (fov, aspect ratio) ?  This is a view matrix, the perspective is in renderer
	void pointCameraAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void turnCameraTowards(DirectX::XMFLOAT3 target, float speed);
	void moveCameraAlongForward(float speed);
	void Update();
};
