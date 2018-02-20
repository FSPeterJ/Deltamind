#include "ComponentBase.h"      // for InstantiatedCompBase, ComponentBase
#include "GameObject.h"         // for GameObject
#include "ObjectManager.h"      // for ObjectManager
#include "MessageEvents.h"
#include "Console.h"

#define MAX_NAME_LENGTH 512
#define MAX_EXTENSION_LENGTH 64
#define MAX_TAG_LENGTH 64

std::unordered_map<unsigned, std::function<Object*(void)>> ObjectFactory::registeredConstructors;
std::unordered_map<unsigned, std::function<Object*(Object*)>> ObjectFactory::registeredCasters;

ObjectManager* ObjectFactory::objMan;

std::unordered_map<std::string, unsigned> ObjectFactory::prefabNames;
std::unordered_map<unsigned, std::string> ObjectFactory::prefabNamesReverse; // For Debugging

std::unordered_map<unsigned, unsigned> ObjectFactory::Object2Prefab;

std::vector<IComponentManager*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;

std::vector<unsigned > ObjectFactory::objectSize;

void ObjectFactory::Initialize(ObjectManager* _objMan, const char* object) {
	objMan = _objMan;
	//assetsFolder = new char[MAX_NAME_LENGTH];
	//strcpy_s(assetsFolder, strlen(object) > MAX_NAME_LENGTH ? MAX_NAME_LENGTH : strlen(object), object);
	//assetsFolderLength = strlen(assetsFolder);
	//int r = TypeMap::GetTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
	MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
	MessageEvents::Subscribe(EVENT_InstantiateRequestByType, InstantiateByType);
	MessageEvents::Subscribe(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateByName);
}

void ObjectFactory::Instantiate(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*)e;
	GameObject* newobject = ActivateObject(instantiate->pid);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}
	memcpy(&newobject->position, &instantiate->position, sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByType(EventMessageBase *e) {
	InstantiateTypeMessage<GameObject>* instantiate = (InstantiateTypeMessage<GameObject>*)e;
	GameObject* newobject;
	if(instantiate->pid) {
		if(prefabs[instantiate->pid].objectTypeID != instantiate->tid) {
			Console::ErrorOutLine << "ObjectFactory: Type Mismatch!  Instantiate Request of " << prefabNamesReverse[instantiate->pid].c_str()  << " has a requesting typeID of " << TypeMap::GetObjectNameFromID(instantiate->tid) << " Which does not match the prefab typeID of " << TypeMap::GetObjectNameFromID(prefabs[instantiate->pid].objectTypeID);
			Console::WarningLine << "ObjectFactory: Type Mismatch!  Instantiate Request of " << prefabNamesReverse[instantiate->pid].c_str() << " has a requesting typeID of " << TypeMap::GetObjectNameFromID(instantiate->tid) << " Which does not match the prefab typeID of " << TypeMap::GetObjectNameFromID(prefabs[instantiate->pid].objectTypeID);
		}
		newobject = ActivateObject(instantiate->pid);

	}
	else {
		newobject = ActivateObject(Object2Prefab[instantiate->tid]);

	}
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}		
	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByName(EventMessageBase *e) {
	InstantiateNameMessage<GameObject>* instantiate = (InstantiateNameMessage<GameObject>*)e;
	PrefabId selectedPrefab = prefabNames[std::string(instantiate->debug_name)];
	if(prefabs[selectedPrefab].objectTypeID != instantiate->tid) {
		Console::ErrorOutLine << "ObjectFactory: Type Mismatch!  Instantiate Request of " << prefabNamesReverse[selectedPrefab].c_str() << " has a requesting typeID of " << TypeMap::GetObjectNameFromID(instantiate->tid) << " Which does not match the prefab typeID of " << TypeMap::GetObjectNameFromID(prefabs[selectedPrefab].objectTypeID);
		Console::WarningLine << "ObjectFactory: Type Mismatch!  Instantiate Request of " << prefabNamesReverse[selectedPrefab].c_str() << " has a requesting typeID of " << TypeMap::GetObjectNameFromID(instantiate->tid)<< " Which does not match the prefab typeID of " << TypeMap::GetObjectNameFromID(prefabs[selectedPrefab].objectTypeID);
	}

	GameObject* newobject = ActivateObject(prefabNames[std::string(instantiate->debug_name)]);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}

	memcpy(&newobject->position, &instantiate->position, sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

GameObject* ObjectFactory::ActivateObject(PrefabId pid) {
	GameObject* newobject = objMan->Instantiate(prefabs[pid].objectTypeID);
	auto TypeGathered = registeredCasters[prefabs[pid].objectTypeID](newobject);
	TypeGathered->CloneData(prefabs[pid].object);
	//newobject->CloneData(prefabs[pid].object);
	for(int i = 0; i < 64; i++) {
		if(prefabs[pid].fastclone[i]) {
			newobject->SetComponent(prefabs[pid].instantiatedComponents[i], i);
		}
		else if(prefabs[pid].instantiatedComponents[i] != nullptr) {
			InstantiatedCompBase* comptemp = (InstantiatedCompBase *)managers[i]->CloneComponent(prefabs[pid].instantiatedComponents[i]);
			comptemp->parentObject = newobject; // This will crash if this is not an InstantiatedCompBase
			newobject->SetComponent(comptemp, i);
			newobject->DestroyComponents.Add([=]() {
				managers[i]->ResetComponent(comptemp);
			});
		}
	}
	//newobject->Enable();
	return newobject;
}

unsigned ObjectFactory::CreatePrefab(std::string* _filename, char* DEBUG_STRING_NAME, bool objectPrefabOverride) {
	int prefabID = prefabNames[*_filename];
	if(prefabID) {
		//This Prefab already exists.
		if(objectPrefabOverride) {
			Object2Prefab[prefabs[prefabID].objectTypeID] = prefabID;
		}
		if(DEBUG_STRING_NAME) {
			prefabNamesReverse[prefabID] = std::string(DEBUG_STRING_NAME);
			prefabNames[std::string(DEBUG_STRING_NAME)] = prefabID;
		}
	}
	else {
		prefabID = (int)prefabs.size();
		prefabs.push_back(Prefab());
		Prefab* prefab = &prefabs[prefabID];
		FILE* file = nullptr;
		fopen_s(&file, _filename->c_str(), "rb");

		int max_fget = 0;
		if(file) {
			//Read ClassName
			int nameLength;

			fread(&nameLength, sizeof(int), 1, file);
			char className[MAX_NAME_LENGTH];
			max_fget = abs(nameLength) + 1;
			// Note the intentional offset by 1
			max_fget = max_fget < MAX_NAME_LENGTH ? max_fget : MAX_NAME_LENGTH;
			fgets(className, max_fget, file);
			prefab->objectTypeID = TypeMap::GetObjectNameID(std::string(className));
			if(prefab->objectTypeID == 0) {
				Console::ErrorOutLine << "ObjectFactory: An unrecognized object class of '" << className << "' was referenced by object '" << _filename->c_str() << "' and was not able to be loaded.  This may cause serious issues.";
				Console::ErrorLine << "ObjectFactory: An unrecognized object class of '" << className << "' was referenced by object '" << _filename->c_str() << "' and was not able to be loaded.  This may cause serious issues.";
			}
			prefab->object = registeredConstructors[prefab->objectTypeID]();
			prefab->size = objectSize[prefab->objectTypeID];
			int dataNameLen = 0;

			while(fread(&dataNameLen, sizeof(int), 1, file)) {
				char componentIdentifier[MAX_NAME_LENGTH] = { 0 };
				max_fget = abs(dataNameLen) + 1;
				max_fget = max_fget < MAX_NAME_LENGTH ? max_fget : MAX_NAME_LENGTH;
				fgets(componentIdentifier, max_fget, file);

				//Handle specific file extension
				char* ext;
				GetFileExtension(componentIdentifier, dataNameLen, (char**)&ext);
				if(ext == nullptr) {
					ext = componentIdentifier;
				}
				int componentTypeID = TypeMap::GetComponentNameID(std::string(ext));
				if(componentTypeID) {

					// Check for a tag
					int tagNameLength;
					fread(&tagNameLength, sizeof(int), 1, file);
					char componentTag[MAX_TAG_LENGTH];
					if(tagNameLength) {
						max_fget = ++tagNameLength < MAX_NAME_LENGTH ? tagNameLength : MAX_NAME_LENGTH;
						fgets(componentTag, max_fget, file);
					}
					//Check for special data flag
					char* componentData = nullptr;
					if(dataNameLen < 0) {
						int componentDataLength = 0;
						fread(&componentDataLength, sizeof(int), 1, file);
						componentData = new char[componentDataLength];
						fread(componentData, componentDataLength, 1, file);
					}

					//I do not know where I want this in the end, I just know I do not want it in the binary file.
					static const char* directory = "Assets/";
					static const size_t length = strlen(directory);
					memmove_s(componentIdentifier + length, MAX_NAME_LENGTH, componentIdentifier, MAX_NAME_LENGTH - length);
					memcpy(componentIdentifier, directory, length);

					// The manager is expected to check if componentData is null
					ComponentBase * component = managers[componentTypeID]->GetReferenceComponent(componentIdentifier, componentData);
					prefab->fastclone[componentTypeID] = component->singleInstance;
					prefab->instantiatedComponents[componentTypeID] = component;
					if(!component->singleInstance) {
						//This assign's the component parent object if the component is unique per object
						((InstantiatedCompBase *)prefab->instantiatedComponents[componentTypeID])->parentObject = prefab->object;
					}
					prefab->object->SetComponent(component, componentTypeID);
					delete[] componentData;
				}
				else if(TypeMap::GetObjectNameID(std::string(ext)))
				{
					static const char* directory = "Assets/";
					static const size_t length = strlen(directory);
					memmove_s(componentIdentifier + length, MAX_NAME_LENGTH, componentIdentifier, MAX_NAME_LENGTH - length);
					memcpy(componentIdentifier, directory, length);
					// In local context, 0 is impossible as it is the first one
					unsigned componentPrefabID = prefabNames[componentIdentifier];
					if(componentPrefabID == 0)
					{
						componentPrefabID = CreatePrefab(&std::string(componentIdentifier));
						prefab = &prefabs[prefabID]; //The vector may move
					}
					// Check for a tag
					int tagNameLength;
					fread(&tagNameLength, sizeof(int), 1, file);
					char componentTag[MAX_TAG_LENGTH];
					if(tagNameLength) {
						max_fget = ++tagNameLength < MAX_NAME_LENGTH ? tagNameLength : MAX_NAME_LENGTH;
						fgets(componentTag, max_fget, file);
					}
					prefab->object->GivePID(componentPrefabID, componentTag);
				}
				else {

					Console::ErrorOutLine << "ObjectFactory: An unrecognized component of '" << componentIdentifier << "' on '" << _filename->c_str() << "' was not able to be loaded.  This may cause serious issues.";
					Console::WarningLine << "ObjectFactory: An unrecognized component of '" << componentIdentifier << "' on '" << _filename->c_str() << "' was not able to be loaded.  This may cause serious issues.";
				}
			}
		}
		if(file != nullptr) {
			fclose(file);
		}
		else {
			Console::ErrorOut << "ObjectFactory: Failed to load the file '" << _filename->c_str() << "'.  This may cause serious issues. \n";
			Console::ErrorLine << "ObjectFactory: Failed to load the file '" << _filename->c_str() << "'.  This may cause serious issues.";
			assert(false);
		}
#ifdef _DEBUG
		prefab->object->SmokeTest();
#endif
		if(objectPrefabOverride) {
			Object2Prefab[prefab->objectTypeID] = prefabID;
		}
		prefabNames[*_filename] = prefabID;
		prefabNamesReverse[prefabID] = *_filename;
		if(DEBUG_STRING_NAME) {
			prefabNamesReverse[prefabID] = std::string(DEBUG_STRING_NAME);
			prefabNames[std::string(DEBUG_STRING_NAME)] = prefabID;
		}
		

	}
	return prefabID;
}

