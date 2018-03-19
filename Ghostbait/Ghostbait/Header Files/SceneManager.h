#pragma once
#include <map>
#include <DirectXMath.h> //TODO: Can I get rid of this?
						 //namespace DirectX {
						 //	class XMFLOAT4X4;
						 //}
class Core;

struct Scene {
	std::string sceneFile;
	std::vector<std::string> levelFiles;

	Scene() : sceneFile("") {}
	Scene(std::string _sceneFile, std::vector<std::string> _levelFile) : sceneFile(_sceneFile), levelFiles(_levelFile) {}
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
		std::vector<std::string> levelName;
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

	const Scene ResetCurrentScene();
	void LoadScene(const char* sceneName, Core** _core = nullptr);
	void LoadScene(Scene& scene, Core** _core = nullptr);
	inline void ReloadScene() { LoadScene(*currentScene); };

	const Scene GetSceneFromName(const char* sceneName);
	const std::string GetNameFromScene(const Scene& scene);
	inline const Scene& GetCurrentScene() const { return *currentScene; };
};

