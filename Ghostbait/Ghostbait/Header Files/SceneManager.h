#pragma once
#include <map>
#include <DirectXMath.h> //TODO: Can I get rid of this?
						 //namespace DirectX {
						 //	class XMFLOAT4X4;
						 //}

struct Scene {
	std::string sceneFile;
	std::string levelFile;

	Scene() : sceneFile(""), levelFile("") {}
	Scene(std::string _sceneFile, std::string _levelFile) : sceneFile(_sceneFile), levelFile(_levelFile) {}
};

class SceneManager {
	struct TestSceneData {
		struct Prefab {
			std::string ghostFile;
			std::string name;
			std::vector<DirectX::XMFLOAT4X4> positions;
		};
		std::string fileName;
		std::string sceneName;
		std::string levelName;
		std::vector<Prefab> prefabs;
	};

	std::map<std::string, Scene> scenes;
	Scene* currentScene = nullptr;

	std::wstring string2wstring(std::string str);
	std::string wstring2string(std::wstring wstr);

	void CreateSceneFile(TestSceneData& data);

	void FetchAllSceneFiles(const char* folderPath = "Scene Files");
public:
	SceneManager(){};
	~SceneManager(){};
	
	void Initialize();

	//---WIP--- These functions could be useful but are not needed at this point in time
	void CreateSceneFromFBX(const char* filePath, const char* sceneName);
	void CreateSceneFromCurrentObjects(const char* filePath, const char* sceneName);
	//---------

	const Scene UnloadScene();
	void LoadScene(const char* sceneName, DirectX::XMFLOAT3* _corePos = nullptr);
	void LoadScene(Scene& scene, DirectX::XMFLOAT3* _corePos = nullptr);
	inline void ReloadScene() { LoadScene(*currentScene); };

	const Scene GetSceneFromName(const char* sceneName);
	const std::string GetNameFromScene(const Scene& scene);
	inline const Scene& GetCurrentScene() const { return *currentScene; };
};

