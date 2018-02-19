#pragma once
#include <map>
#include <vector>
#include <DirectXMath.h> //TODO: Can I get rid of this?
						 //namespace DirectX {
						 //	class XMFLOAT4X4;
						 //}

struct Scene {
	std::vector<unsigned int> prefabIDs = std::vector<unsigned int>();
	std::vector<DirectX::XMFLOAT4X4> matrices = std::vector<DirectX::XMFLOAT4X4>();
};

class SceneManager {
	std::map<std::string, Scene> scenes;

	void FetchAllSceneFiles();
public:
	SceneManager(){};
	~SceneManager(){};
	
	void Initialize();

	void LoadScene(const char* sceneName);
	void LoadScene(const Scene scene);

	const Scene GetScene(const char* sceneName);
};

