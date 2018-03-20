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
		Writer::WriteInt((int)data.levelName.size());
		for (size_t i = 0; i < data.levelName.size(); ++i)
			Writer::WriteIntString(data.levelName[i]);

		Writer::WriteInt((int)data.prefabs.size());
		for (size_t i = 0; i < data.prefabs.size(); ++i) {
			Writer::WriteIntString(data.prefabs[i].ghostFile);
			Writer::WriteInt(data.prefabs[i].positions.size());
			for (size_t j = 0; j < data.prefabs[i].positions.size(); ++j) {
				Writer::WriteMatrix(data.prefabs[i].positions[j]);
			}
		}
	}
	file.close();
}

void SceneManager::Initialize() {
	DirectX::XMFLOAT4X4 identity = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	TestSceneData level0Data;
	{
		level0Data.fileName = "Scene Files//level0.scene";

		level0Data.sceneName = "level0";
		level0Data.levelName.push_back(std::string("Level Files//level0.xml"));
		TestSceneData::Prefab ground;
		{
			ground.ghostFile = "Assets/SpaceshipHull.ghost";

			ground.name = "Ground";
			//DirectX::XMFLOAT4X4 mat;
			//DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixScaling(0.5f, 0.35f, 0.5f));
			ground.positions.push_back(identity);
			level0Data.prefabs.push_back(ground);
		}
		TestSceneData::Prefab startCube;
		{
			startCube.ghostFile = "Assets/StartCube.ghost";
			startCube.name = "StartCube";
			DirectX::XMFLOAT4X4 mat = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 1.5f, 3, 1);
			startCube.positions.push_back(mat);
			level0Data.prefabs.push_back(startCube);
		}
		TestSceneData::Prefab spawner;
		{
			spawner.ghostFile = "Assets/Spawner.ghost";
			spawner.name = "Spawner";
			DirectX::XMFLOAT4X4 mat1 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, -50, 0, 50, 1);
			spawner.positions.push_back(mat1);
			DirectX::XMFLOAT4X4 mat2 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, -50, 0, -50, 1);
			spawner.positions.push_back(mat2);
			DirectX::XMFLOAT4X4 mat3 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, 50, 0, 50, 1);
			spawner.positions.push_back(mat3);
			DirectX::XMFLOAT4X4 mat4 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, 50, 0, -50, 1);
			spawner.positions.push_back(mat4);
			DirectX::XMFLOAT4X4 mat5 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, 50, 0, 0, 1);
			spawner.positions.push_back(mat5);
			DirectX::XMFLOAT4X4 mat6 = DirectX::XMFLOAT4X4(0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0.5f, 0, -50, 0, 0, 1);
			spawner.positions.push_back(mat6);
			level0Data.prefabs.push_back(spawner);
		}
		TestSceneData::Prefab winCube;
		{
			winCube.ghostFile = "Assets/WinCube.ghost";
			winCube.name = "WinCube";
			level0Data.prefabs.push_back(winCube);
		}
		TestSceneData::Prefab coreCube;
		{
			coreCube.ghostFile = "Assets/CoreCube.ghost";
			coreCube.name = "CoreCube";
			DirectX::XMFLOAT4X4 mat1 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 5.25f, 1);
			coreCube.positions.push_back(mat1);
			level0Data.prefabs.push_back(coreCube);
		}
	}
	CreateSceneFile(level0Data);
	/*
	TestSceneData splashScreenData;
	{
		splashScreenData.fileName = "Scene Files//splashScreen.scene";

		splashScreenData.sceneName = "splashScreen";
		splashScreenData.levelName.push_back(std::string("Level Files//splashScreen.xml"));
		//TestSceneData::Prefab coreCube;
		//{
		//	coreCube.ghostFile = "Assets/ElonMuskQuad.ghost";
		//	coreCube.name = "MuskQuad";
		//	DirectX::XMFLOAT4X4 mat1 = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 5, 1);
		//	coreCube.positions.push_back(mat1);
		//	splashScreenData.prefabs.push_back(coreCube);
		//}
	}
	CreateSceneFile(splashScreenData);
	TestSceneData mainMenuData;
	{
		mainMenuData.fileName = "Scene Files//mainMenu.scene";

		mainMenuData.sceneName = "mainMenu";
		TestSceneData::Prefab tutorialQuad;
		{
			tutorialQuad.ghostFile = "Assets/VRTutorialQuad.ghost";
			tutorialQuad.name = "TutorialQuad";
			DirectX::XMFLOAT4X4 mat1;
			DirectX::XMMATRIX rotation = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180)), DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(90)));
			DirectX::XMStoreFloat4x4(&mat1, rotation);
			mat1._41 = 0;
			mat1._42 = 1;
			mat1._43 = -3;

			tutorialQuad.positions.push_back(mat1);
			mainMenuData.prefabs.push_back(tutorialQuad);
		}
	}
	CreateSceneFile(mainMenuData);

	*/

	//Fill map of scenes using the ".scene" files from our "Scene Files" directory
	FetchAllSceneFiles();
}

void SceneManager::FetchAllSceneFiles(const char* folderPath) {


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
	for (size_t i = 0; i < paths.size(); ++i) {
		//2. Read first two values of each file
		std::ifstream file(paths[i], std::ios::binary);
		if (file.is_open()) {
			Reader::defaultStream = &file;
			int nameLength = Reader::ReadInt();
			std::string name = Reader::ReadString(nameLength);
			lastName = name;
			std::vector<std::string> levels;
			int levelAmount = Reader::ReadInt();
			for (int index = 0; index < levelAmount; ++index)
			{
				int levelLength = Reader::ReadInt();
				std::string level = Reader::ReadString(levelLength);
				levels.push_back(level);
			}

			Scene newScene = Scene(paths[i], levels);
			scenes[name] = newScene;
		}
		file.close();
	}
}

const Scene SceneManager::ResetCurrentScene() {
	//Destroy everything

	//Reset currentScene
	Scene prevScene = *currentScene;
	currentScene = nullptr;
	return prevScene;
}

void SceneManager::LoadScene(const char* sceneName, Core** _core) {
	Scene& scene = scenes[sceneName];
	if (scene.sceneFile == "")
		Console::ErrorLine << "Trying to load a scene named " << sceneName << " that does not exist in SceneManager!";
	else {
		//UnloadScene();
		currentScene = &scene;

		std::ifstream file(scene.sceneFile, std::ios::binary);
		if (file.is_open()) {
			Reader::defaultStream = &file;
			int nameLen = Reader::ReadInt();
			std::string name = Reader::ReadString(nameLen);

			int levelCount = Reader::ReadInt();
			for (int i = 0; i < levelCount; ++i)
			{
				int levelLen = Reader::ReadInt();
				std::string level = Reader::ReadString(levelLen);
			}

			int pCount = Reader::ReadInt();
			for (int i = 0; i < pCount; ++i)
			{
				int ghostLen = Reader::ReadInt();
				std::string ghostName = Reader::ReadString(ghostLen);
				unsigned int prefabID = ObjectFactory::CreatePrefab(&ghostName);

				int ghostCount = Reader::ReadInt();
				for (int i = 0; i < ghostCount; ++i) {
					DirectX::XMFLOAT4X4 mat = Reader::ReadMatrix();
					MessageEvents::SendQueueMessage(EVENT_Late, [=] {  
						GameObject* newObj;
						MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0, 0, 0 }, &newObj));
						if (_core && !strcmp(newObj->GetTag().c_str(), "Core")) {
							*_core = (Core*)newObj;
						}
						newObj->transform.SetMatrix(mat);
						newObj->Enable();
						});
				}
				//Will be garbage if the file is empty. Should only be garbage on the last call
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
void SceneManager::LoadScene(Scene& scene, Core** _core) {
	if (scene.sceneFile == "") {
		Console::ErrorLine << "Trying to load an unknown scene that does not exist in SceneManager!";
		return;
	}
	LoadScene(GetNameFromScene(scene).c_str(), _core);
}

const Scene SceneManager::GetSceneFromName(const char* sceneName) {
	if (scenes.find(sceneName) != scenes.end())
		return scenes[sceneName];
	return Scene();
}
const std::string SceneManager::GetNameFromScene(const Scene& scene) {
	for (auto e : scenes) {
		if (e.second.sceneFile == scene.sceneFile &&
			e.second.levelFiles == scene.levelFiles) {
			return e.first;
		}
	}
	return std::string("");
}
