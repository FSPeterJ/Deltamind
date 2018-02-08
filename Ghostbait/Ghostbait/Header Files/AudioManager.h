#pragma once
#include "Camera.h"
#include "GameObject.h"
#include "MessageStructs.h"
#include <DirectXMath.h>
#include <vector>

#ifdef NDEBUG
#define AK_OPTIMIZED
#endif // !NDEBUG

class AudioManager
{
	const DirectX::XMFLOAT4X4* cam;
	std::vector<GameObject*> objects;
public:
	AudioManager();
	~AudioManager();

	void setCamera(const DirectX::XMFLOAT4X4* _camera);
	void registerObject(EventMessageBase* e);
	void unRegisterObject(EventMessageBase* e);
	void playSound(EventMessageBase* e);

	void Update();
};

