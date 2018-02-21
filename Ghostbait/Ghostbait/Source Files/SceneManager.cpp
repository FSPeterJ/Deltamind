#include <vector>
#include <fstream>
#include <Windows.h>
#include "SceneManager.h"
#include "Console.h"
#include "GameObject.h"
#include "MessageEvents.h"
#include "MessageStructs.h"
#include "ObjectFactory.h"
#include "BinaryFileIO.h"

std::wstring SceneManager::string2wstring(std::string str) {
	int len;
	int strlength = (int)str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), strlength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), strlength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
std::string SceneManager::wstring2string(std::wstring wstr) {
	int len;
	int wstrlength = (int)wstr.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstrlength, 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstrlength, buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;
}

void SceneManager::CreateSceneFile(SceneManager::TestSceneData& data) {
	std::ofstream file(data.fileName, std::ios::binary);

	if (file.is_open()) {
		Writer::defaultStream = &file;

		Writer::WriteIntString(data.sceneName);
		Writer::WriteIntString(data.levelName);
		
		for (int i = 0; i < data.prefabs.size(); ++i) {
			Writer::WriteIntString(data.prefabs[i].ghostFile);
 			Writer::WriteInt(data.prefabs[i].positions.size());
			for (int j = 0; j < data.prefabs[i].positions.size(); ++j) {
				Writer::WriteMatrix(data.prefabs[i].positions[j]);
			}
		}
	}
	file.close();
}

void SceneManager::Initialize() {
	//TestSceneData data;
	//{
	//	data.fileName = "Scene Files//level0.scene";
	//
	//	data.sceneName = "level0";
	//	data.levelName = "Level Files//level0.xml";
	//	TestSceneData::Prefab prefab1;
	//	prefab1.ghostFile = "Assets//basicSphere.ghost";
	//	DirectX::XMFLOAT4X4 mat1 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1);
	//	prefab1.positions.push_back(mat1);
	//	DirectX::XMFLOAT4X4 mat2 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1);
	//	prefab1.positions.push_back(mat2);
	//	DirectX::XMFLOAT4X4 mat3 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1);
	//	prefab1.positions.push_back(mat3);
	//	DirectX::XMFLOAT4X4 mat4 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1);
	//	prefab1.positions.push_back(mat4);
	//	data.prefabs.push_back(prefab1);
	//}
	//CreateTestSceneFile(data);
	//
	//Fill map of scenes using the ".scene" files from our "Scene Files" directory
	FetchAllSceneFiles();
}

void SceneManager::FetchAllSceneFiles(const char* folderPath) {
	
	//3. Use values to create a new scene and store it

	//1. Find all files in directory that end in .ghost
	std::vector<std::string> paths;
	{
		std::string pattern(folderPath);
		pattern.append("//*.scene");
		WIN32_FIND_DATA data;
		HANDLE hFind;
		if ((hFind = FindFirstFile(string2wstring(pattern).c_str(), &data)) != INVALID_HANDLE_VALUE) {
			do {
				std::string fileName = wstring2string(std::wstring(data.cFileName));
				std::string fullPath(folderPath);
				fullPath.append("//");
				fullPath.append(fileName);
				paths.push_back(fullPath);
			} while (FindNextFile(hFind, &data) != 0);
			FindClose(hFind);
		}
	}

	if (!paths.size()) {
		Console::WarningLine << "No scene files found!";
		return;
	}
	std::string lastName;
	for (int i = 0; i < paths.size(); ++i) {
		//2. Read first two values of each file
		std::ifstream file(paths[i], std::ios::binary);
		if (file.is_open()) {
			Reader::defaultStream = &file;
			int nameLength = Reader::ReadInt();
			std::string name = Reader::ReadString(nameLength);
			lastName = name;
			int levelLength = Reader::ReadInt();
			std::string level = Reader::ReadString(levelLength);
			
			Scene newScene = Scene(paths[i], level);
			scenes[name] = newScene;
		}
		file.close();
	}
}

void SceneManager::UnloadScene() {
	//Destroy everything and reset object factories prefabs/pools
	
	//Reset currentScene
	currentScene = nullptr;
}

void SceneManager::LoadScene(const char* sceneName) {
	Scene& scene = scenes[sceneName];
	if (scene.sceneFile == "")
		Console::ErrorLine << "Trying to load a scene named " << sceneName << " that does not exist in SceneManager!";
	else {
		UnloadScene();
		currentScene = &scene;

		std::ifstream file(scene.sceneFile, std::ios::binary);
		if (file.is_open()) {
			Reader::defaultStream = &file;
			int nameLen = Reader::ReadInt();
			std::string name = Reader::ReadString(nameLen);

			int levelLen = Reader::ReadInt();
			std::string level = Reader::ReadString(levelLen);

			int ghostLen = Reader::ReadInt();
			while (!file.eof()) {
				std::string ghostName = Reader::ReadString(ghostLen);
				unsigned int prefabID = ObjectFactory::CreatePrefab(&std::string(ghostName));

				int ghostCount = Reader::ReadInt();
				for (int i = 0; i < ghostCount; ++i) {
					DirectX::XMFLOAT4X4 mat = Reader::ReadMatrix();
					GameObject* newObj;
					MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0, 0, 0 }, &newObj));
					newObj->position = mat;
				}
				//Will be garbage if the file is empty. Should only be garbage on the last call
				ghostLen = Reader::ReadInt();
			}

		}
		else {
			Console::ErrorLine << "Failed to open the scene file " << scene.sceneFile << "!";
			return;
		}
		file.close();
	}
	return;

}
void SceneManager::LoadScene(Scene& scene) {
	if (scene.sceneFile == "") {
		Console::ErrorLine << "Trying to load an unknown scene that does not exist in SceneManager!";
		return;
	}
	LoadScene(GetNameFromScene(scene).c_str());
}

const Scene SceneManager::GetSceneFromName(const char* sceneName) {
	if(scenes.find(sceneName) != scenes.end())
		return scenes[sceneName];
	return Scene();
}
const std::string SceneManager::GetNameFromScene(const Scene& scene) {
	for (auto e : scenes) {
		if (e.second.sceneFile == scene.sceneFile &&
			e.second.levelFile == scene.levelFile) {
			return e.first;
		}
	}
	return std::string("");
}
