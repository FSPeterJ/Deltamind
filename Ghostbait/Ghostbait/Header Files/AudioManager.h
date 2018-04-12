#pragma once
#include "Camera.h"
#include "GameObject.h"
#include "MessageStructs.h"
#include <vector>

#ifdef NDEBUG
#define AK_OPTIMIZED
#endif // !NDEBUG


namespace DirectX
{
	struct XMFLOAT4X4;
}

class AudioManager
{
	const DirectX::XMFLOAT4X4* cam;
	std::vector<GameObject*> objects;
public:
	AudioManager();
	~AudioManager();

	void setCamera(const DirectX::XMFLOAT4X4* _camera);
	void LoadBanks();
	void registerObject(EventMessageBase* e);
	void unRegisterObject(EventMessageBase* e);
	void playSound(EventMessageBase* e);

	void Update();
};

