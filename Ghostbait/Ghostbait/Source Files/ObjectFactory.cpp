#include "ComponentBase.h"      // for InstantiatedCompBase, ComponentBase
#include "GameObject.h"         // for GameObject
#include "ObjectManager.h"      // for ObjectManager
#include "MessageEvents.h"

#define MAX_NAME_LENGTH 512
#define MAX_EXTENSION_LENGTH 64
#define MAX_TAG_LENGTH 64

std::unordered_map<unsigned, std::function<Object*(void)>> ObjectFactory::registeredConstructors;

ObjectManager* ObjectFactory::objMan;

std::unordered_map<std::string, unsigned> ObjectFactory::prefabNames;

std::unordered_map<unsigned, unsigned> ObjectFactory::Object2Prefab;

std::vector<IComponentManager*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;

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
	GameObject* newobject = ActivateObject(instantiate->GetPrefabId());
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}
	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByType(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*)e;
	GameObject* newobject = ActivateObject(Object2Prefab[instantiate->GetPrefabId()]);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}

	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByName(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*)e;
	GameObject* newobject = ActivateObject(Object2Prefab[instantiate->GetPrefabId()]);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}

	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

GameObject* ObjectFactory::ActivateObject(PrefabId pid) {
	GameObject* newobject = objMan->Instantiate(prefabs[pid].objectTypeID);

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
	newobject->Enable();
	return newobject;
}

void ObjectFactory::CreatePrefab(std::string *_filename, char* DEBUG_STRING_NAME, bool objectPrefabOverride) {
	int prefabID = prefabNames[*_filename];
	if(prefabID) {
		//This Prefab already exists.
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
			// Note the intentional offset by 1
			max_fget = ++nameLength < MAX_NAME_LENGTH ? nameLength : MAX_NAME_LENGTH;
			fgets(className, max_fget, file);
			prefab->objectTypeID = TypeMap::GetObjectNameID(std::string(className));
			prefab->object = registeredConstructors[prefab->objectTypeID]();
			int dataNameLen;

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

				// Check for a tag
				int tagNameLength;
				fread(&tagNameLength, sizeof(int), 1, file);
				if(tagNameLength) {
					char componentTag[MAX_TAG_LENGTH];
					max_fget = tagNameLength < MAX_NAME_LENGTH ? tagNameLength : MAX_NAME_LENGTH;
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
				prefab->instantiatedComponents[componentTypeID] = component;
				prefab->fastclone[componentTypeID] = component->singleInstance;
				if(component->singleInstance) {
					//This assign's the component parent object if the component is unique per object
					((InstantiatedCompBase *)prefab->instantiatedComponents[componentTypeID])->parentObject = prefab->object;
				}
				prefab->object->SetComponent(component, componentTypeID);
				delete[] componentData;
			}
		}
		fclose(file);
		if(objectPrefabOverride) {
			Object2Prefab[prefab->objectTypeID] = prefabID;
		}
		prefabNames[*_filename] = prefabID;
		if(DEBUG_STRING_NAME) {
			prefabNames[std::string(DEBUG_STRING_NAME)] = prefabID;
		}
	}
}

