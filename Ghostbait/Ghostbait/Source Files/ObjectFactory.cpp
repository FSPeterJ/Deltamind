#include "ObjectFactory.h"
#include <DirectXMath.h>        // for XMFLOAT4X4
#include "ComponentBase.h"      // for InstantiatedCompBase, ComponentBase
#include "Delegate.h"           // for Delegate
#include "GameObject.h"         // for GameObject
#include "IComponentManager.h"  // for IComponentManager
#include "MessageStructs.h"     // for InstantiateMessage, EventMessageBase (ptr only)
#include "ObjectManager.h"      // for ObjectManager
#include "TypeMapping.h"        // for TypeMap
#include "MessageEvents.h"


std::unordered_map<unsigned, std::function<Object*(void)>> ObjectFactory::registeredConstructors;

ObjectManager* ObjectFactory::objMan;

std::unordered_map<std::string, unsigned> ObjectFactory::prefabNames;

std::unordered_map<unsigned, unsigned> ObjectFactory::Object2Prefab;

std::vector<IComponentManager*> ObjectFactory::managers;

std::vector<ObjectFactory::Prefab> ObjectFactory::prefabs;


void ObjectFactory::Initialize(ObjectManager* _objMan) {
	objMan = _objMan;
	//int r = TypeMap::GetTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
	MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
	MessageEvents::Subscribe(EVENT_InstantiateRequestByType, InstantiateByType);
}


void ObjectFactory::Instantiate(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	GameObject* newobject = ActivateObject(instantiate->GetPrefabId());
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}
	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByType(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	GameObject* newobject = ActivateObject(Object2Prefab[instantiate->GetPrefabId()]);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}

	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectFactory::InstantiateByName(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	GameObject* newobject = ActivateObject(Object2Prefab[instantiate->GetPrefabId()]);
	if(instantiate->obj != nullptr) {
		*instantiate->obj = newobject;
	}

	memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}


GameObject* ObjectFactory::ActivateObject(unsigned pid) {
	GameObject* newobject = objMan->Instantiate(prefabs[pid].objectTypeID);

	for(int i = 0; i < 64; i++) {

		if(prefabs[pid].fastclone[i]) {
			newobject->SetComponent(prefabs[pid].instantiatedComponents[i], i);
		} else if(prefabs[pid].instantiatedComponents[i] != nullptr) {
			InstantiatedCompBase* comptemp = (InstantiatedCompBase *) managers[i]->CloneComponent(prefabs[pid].instantiatedComponents[i]);
			comptemp->parentObject = newobject; // This will crash if this is not an InstantiatedCompBase
			newobject->SetComponent(comptemp, i);
			newobject->DestroyComponents.Add([=]() {
				managers[i]->ResetComponent(comptemp);
			});
		}
	}
	return newobject;
}


void ObjectFactory::CreatePrefab(std::string *_filename, char* DEBUG_STRING_NAME, bool objectPrefabOverride) {

	int prefabID = prefabNames[*_filename];
	if(prefabID) {
		//This Prefab already exists.
	} else {
		prefabID = (int) prefabs.size();
		prefabs.push_back(Prefab());
		Prefab* prefab = &prefabs[prefabID];
		FILE* file = nullptr;
		fopen_s(&file, _filename->c_str(), "rb");
		if(file) {
			//Read ClassName
			int nameLength;
			fread(&nameLength, sizeof(int), 1, file);
			char className[512];
			fgets(className, nameLength + 1, file); //TODO: possible buffer overrun with fgets, nameLength is used without being checked
			prefab->objectTypeID = TypeMap::GetObjectNameID(std::string(className));
			prefab->object = registeredConstructors[prefab->objectTypeID]();
			int dataNameLen;
			while(fread(&dataNameLen, sizeof(int), 1, file)) {
				//Check for special flag
				if(dataNameLen > 0) {

					char dataName[512];
					fgets(dataName, dataNameLen + 1, file);
					//Handle specific file extension
					char* ext;
					GetFileExtension(dataName, dataNameLen, (char**) &ext);
					if(ext == nullptr) {
						strcpy_s(ext, 64, dataName);
					}
					int componentTypeID = TypeMap::GetComponentNameID(std::string(ext));

					ComponentBase * component = managers[componentTypeID]->GetReferenceComponent(dataName, nullptr);
					prefab->instantiatedComponents[componentTypeID] = component;
					prefab->fastclone[componentTypeID] = component->singleInstance;
					prefab->object->SetComponent(component, componentTypeID);

				} else {
					int compNameLen;
					fread(&compNameLen, sizeof(int), 1, file);
					//Get component to send data to
					char componentName[512];
					fgets(componentName, compNameLen + 1, file); //TODO: possible buffer overrun with fgets, compNameLen is used without being checked

					char* ext;
					GetFileExtension(componentName, compNameLen, (char**) &ext);
					unsigned componentTypeID;// = TypeMap::GetComponentNameID(std::string(ext)); //TODO: above, ext is used as a null in/out variable, so it is possible for it to become null when passed into std::string
					if(ext == nullptr)
						componentTypeID = TypeMap::GetComponentNameID(std::string(componentName));
					else
						componentTypeID = TypeMap::GetComponentNameID(std::string(ext));


					//Get data to send
					char* compData = new char[-dataNameLen];
					fread(compData, -dataNameLen, 1, file);
					ComponentBase * component = managers[componentTypeID]->GetReferenceComponent(componentName, compData);

					prefab->instantiatedComponents[componentTypeID] = component;
					prefab->fastclone[componentTypeID] = component->singleInstance;
					if(component->singleInstance) {
						((InstantiatedCompBase *) prefab->instantiatedComponents[componentTypeID])->parentObject = prefab->object;
					}
					prefab->object->SetComponent(prefab->instantiatedComponents[componentTypeID], componentTypeID);
					delete[] compData; //TODO: Not sure if delete[] or delete

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
}
