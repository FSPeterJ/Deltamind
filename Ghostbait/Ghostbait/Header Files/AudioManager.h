#pragma once
#include "Camera.h"
#include "GameObject.h"
#include <vector>

class AudioManager
{
	const Camera* cam;
	std::vector<GameObject*> objects;
public:
	AudioManager();
	~AudioManager();

	void setCamera(const Camera* _camera);
	
	void Update();
};

