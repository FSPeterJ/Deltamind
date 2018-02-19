#include "SceneManager.h"
#include "Console.h"
#include "GameObject.h"
#include "MessageEvents.h"
#include "MessageStructs.h"

void SceneManager::FetchAllSceneFiles() {

}

void SceneManager::Initialize() {
	FetchAllSceneFiles();
}

void SceneManager::LoadScene(const char* sceneName) {
	Scene scene = GetScene(sceneName);
	if (scene.prefabIDs.size() > 0)
		LoadScene(scene);
	else
		Console::ErrorLine << "Trying to get load a scene named " << sceneName << " that does not exist in SceneManager!";
	return;

}
void SceneManager::LoadScene(const Scene scene) {
	if (scene.prefabIDs.size() == 0) {
		Console::ErrorLine << "Trying to get load an unknown scene that does not exist in SceneManager!";
		return;
	}
	for (int i = 0; i < scene.prefabIDs.size(); ++i) {
		GameObject* newObj;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(scene.prefabIDs[i], { 0, 0, 0 }, &newObj));
		newObj->position = scene.matrices[i];
	}
}

const Scene SceneManager::GetScene(const char* sceneName) {
	if(scenes.find(sceneName) != scenes.end())
		return scenes[sceneName];
	return Scene();
}