#pragma once
#include <vector>
#include <unordered_map>
#include <bitset>
#include "Object.h"
#include "ObjectManager.h"
#include "TypeMapping.h"
#include "ComponentBase.h"

/// <summary>
/// Creates and manages prefabs loaded from the disk.
/// </summary>
class ObjectFactory {
	static ObjectManager* objMan;

	struct Prefab {

	private:
		const static unsigned MAX_DATA = 64;
	public:
		ComponentBase * instantiatedComponents[MAX_DATA] = {};
		unsigned managers[MAX_DATA] = {};
		Object* object = nullptr;
		std::bitset<MAX_DATA> fastclone;
		unsigned objectTypeID = UINT_MAX;
	};

	/// <summary>
	/// Translates a typename's constructor for Objects
	/// </summary>
	template <typename T>
	static Object* ConstructorFunc() {
		return new T;
	}
	ObjectFactory() {};

	static std::unordered_map<unsigned, std::function<Object*(void)>> registeredConstructors;

	static std::vector<IComponentManager*> managers;

	//static std::unordered_map<std::string, IComponentManager*> managerNames;

	//map Names to prefabs
	static std::unordered_map<std::string, unsigned> prefabNames;
	static std::unordered_map<unsigned, unsigned> Object2Prefab;

	//static std::unordered_map<int, Object*> prefabs;
	//pointer storage for prefabs, access by Prefab ID
	static std::vector<Prefab> prefabs;

	// managers
public:
	// Singleton?
	//static ObjectFactory * ObjectFactory::Instance()
	//{
	//	static ObjectFactory factory;
	//	return &factory;
	//}

	/// <summary>
	/// Initializes the Object Factory and hands off the managers it needs to access
	/// </summary>
	static void Initialize(ObjectManager* _objMan) {
		objMan = _objMan;
		//int r = TypeMap::GetTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
		MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
		MessageEvents::Subscribe(EVENT_InstantiateRequestByType, InstantiateByType);
	}

	~ObjectFactory() {};

	/// <summary>
	/// Registeres the constructor of a given object and it's object type ID (class) in the factory
	/// </summary>
	/// <param name="_id">Class ID to register.</param>
	/// <param name="ioClassname">String stored in Ghostbait files that determine which class the data is supposed to go in.
	///	Changing this value means you MUST change this in every ghostbait file. 
	/// </param>
	template <typename ObjectType>
	static void RegisterPrefabBase() {
		registeredConstructors[TypeMap::GetObjectTypeID<ObjectType>()] = &ConstructorFunc<ObjectType>;
		objMan->CreatePool<ObjectType>();
	}

	template <typename ComponentType, typename ManagerType>
	static void RegisterManager(IComponentManager * manager) {
		//Mess
		const int tid = TypeMap::GetComponentTypeID<ComponentType>();
		if(managers.size() <= tid) {
			managers.resize(tid + 1);
		}
		managers[tid] = manager;
	}

	/// <summary>
	/// Creates a Prefab in the Prefab pool from a file if it doesn't exist already
	/// </summary>
	/// <param name="_filename">name of the file to load.</param>

	static int GetFileExtension(char* path, int len, char** ext) {
		*ext = nullptr;
		int extSize = 0;
		for(int i = len - 1; i >= 0; --i) {
			if(path[i] == '.') {
				*ext = &path[i + 1];
			}
			++extSize;
		}
		return extSize;
	}



	static void CreatePrefab(std::string *_filename, bool objectPrefabOverride = false) {

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
						GetFileExtension(dataName, dataNameLen, (char**)&ext);
						if(ext == nullptr)
						{
							strcpy_s(ext, 64, dataName);
						}
						int componentTypeID = TypeMap::GetComponentNameID(std::string(ext));

						ComponentBase * component = managers[componentTypeID]->GetReferenceComponent(dataName, nullptr);
						prefab->instantiatedComponents[componentTypeID] = component;
						prefab->fastclone[componentTypeID] = component->singleInstance;
						prefab->object->SetComponent(component, componentTypeID);

					}
					else {
						int compNameLen;
						fread(&compNameLen, sizeof(int), 1, file);
						//Get component to send data to
						char componentName[512];
						fgets(componentName, compNameLen + 1, file); //TODO: possible buffer overrun with fgets, compNameLen is used without being checked

						char* ext;
						GetFileExtension(componentName, compNameLen, (char**)&ext);
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
						if(component->singleInstance)
						{
							((InstantiatedCompBase *)prefab->instantiatedComponents[componentTypeID])->parentObject = prefab->object;
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
			}
		}
	}


	static void Instantiate(EventMessageBase *e) {
		InstantiateMessage* instantiate = (InstantiateMessage*)e;
		Object* newobject = ActivateObject(instantiate->GetPrefabId());
		if(instantiate->GetReturnObject() != nullptr) {
			instantiate->SetReturnObject(newobject);
		}
		memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
		MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
	}


	static void InstantiateByType(EventMessageBase *e) {
		InstantiateMessage* instantiate = (InstantiateMessage*)e;
		Object* newobject = ActivateObject(Object2Prefab[instantiate->GetPrefabId()]);
		if(instantiate->GetReturnObject() != nullptr) {
			instantiate->SetReturnObject(newobject);
		}

		memcpy(&newobject->position, &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4X4));
		MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
	}

	static Object* ActivateObject(unsigned pid)
	{
		Object* newobject = objMan->Instantiate(prefabs[pid].objectTypeID);

		for(int i = 0; i < 64; i++) {
			if(prefabs[pid].fastclone[i]) {
				newobject->SetComponent(prefabs[pid].instantiatedComponents[i], i);
			}
			else if(prefabs[pid].instantiatedComponents[i] != nullptr) {
				InstantiatedCompBase* comptemp = (InstantiatedCompBase *)managers[i]->CloneComponent(prefabs[pid].instantiatedComponents[i]);
				comptemp->parentObject = newobject; // This will crash if this is not an InstantiatedCompBase
				newobject->SetComponent(comptemp, i);
			}
		}
		return newobject;
	}

	/// <summary>
	/// gives an immutable pointer to the requested Prefab
	/// </summary>
	/// <param name="_pid">Prefab id</param>
	static const Object *  RequestPrefab(const PrefabId _pid) {
		return prefabs[_pid].object;
	}

	/// <summary>
	/// Gets the count of prefabs
	/// </summary>
	static size_t GetPrefabCount() {
		return prefabs.size();
	}

	static void Shutdown() {
		for(auto &prefab : prefabs) {
			delete prefab.object;
		}
	}
};
