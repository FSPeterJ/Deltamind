#pragma once
#include "Object.h"
#include <unordered_map>
#include "MeshManager.h"
#include "ObjectManager.h"
#include "TypeMapping.h"
#include <vector>
#include <bitset>

/// <summary>
/// Creates and manages prefabs loaded from the disk.
/// </summary>
class ObjectFactory {
	static ObjectManager* objMan;

	struct Prefab {
	private:
		const static int MAX_DATA = 64;
	public:
		ComponentBase* instantiatedComponents[MAX_DATA] = {};
		int managers[MAX_DATA] = {};
		Object* object = nullptr;
		std::bitset<MAX_DATA> fastclone;
		int typeID = INT_MAX;
	};

	/// <summary>
	/// Translates a typename's constructor for Objects
	/// </summary>
	template <typename T>
	static Object* ConstructorFunc() {
		return new T;
	}
	ObjectFactory() {};

	static std::unordered_map<int, std::function<Object*(void)>> registeredConstructors;

	static std::vector<IComponentManager*> managers;

	//static std::unordered_map<std::string, IComponentManager*> managerNames;

	//map Names to prefabs
	static std::unordered_map<std::string, int> prefabNames;

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
		//int r = TypeMap::getTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
		MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
	}

	~ObjectFactory() {};

	/// <summary>
	/// Registeres the constructor of a given object and it's object type ID (class) in the factory
	/// </summary>
	/// <param name="_id">Class ID to register.</param>
	template <typename ObjectType>
	static void RegisterPrefabBase() {
		registeredConstructors[TypeMap::getTypeId<ObjectType>()] = &ConstructorFunc<ObjectType>;
		objMan->CreatePool<ObjectType>();
	}

	template <typename ComponentType, typename ManagerType>
	static void RegisterManager(IComponentManager * manager) {
		//Mess
		const int tid = TypeMap::getTypeId<ComponentType>();
		if(managers.size() <= tid) {
			managers.resize(tid + 1);
		}
		managers[tid] = manager;
	}

	/// <summary>
	/// Creates a Prefab in the Prefab pool from a file if it doesn't exist already
	/// </summary>
	/// <param name="_filename">name of the file to load.</param>

	static void GetFileExtension(char* path, int len, char** ext) {
		*ext = nullptr;
		int extSize = 0;
		for (int i = len - 1; i >= 0; --i) {
			if (path[i] == '.') {
				if (!extSize) return;
				*ext = new char[extSize];
				memcpy(*ext, &path[i + 1], extSize);
			}
			++extSize;
		}
		return;
	}

	static void CreatePrefab(std::string *_filename) {
		int prefabID = prefabNames[*_filename];
		if(prefabID) {
			//This Prefab already exists.
		} else {
			prefabID = (int) prefabs.size();
			prefabs.push_back(Prefab());
			Prefab* prefab = &prefabs[prefabID];
			int ObjectType = 0;
			FILE* file = nullptr;
			fopen_s(&file, _filename->c_str(), "rb");
			if(file) {
				//------------Leaking memory temporarily. Will be fixed when we actually store that char*'s-------------//
				//Read ClassName
				int nameLength;
				fread(&nameLength, sizeof(int), 1, file);
				char* className = new char[nameLength];
				fread(className, nameLength, 1, file);

				int dataLen; fread(&dataLen, sizeof(int), 1, file);
				while (!feof(file)) {
					if (dataLen > 0) {
						char* str = new char[dataLen]; 
						fread(str, dataLen, 1, file);
						//TODO: Handle specific file extension
						char* ext; GetFileExtension(str, dataLen, &ext);
						if (ext == "mesh") {

						}
						else if (ext == "mat") {

						}
						else if (ext == "bind") {
						
						}
						else if (ext == "anim") {
						
						}
						else if (ext == "mp3" || ext == "wav") {
							
						}
					}
					else {
						int compLen; fread(&compLen, sizeof(int), 1, file);
						//Get component to send data to
						char* str = new char[compLen];
						fread(str, compLen, 1, file);
						//Get data to send
						char* compData = new char[-dataLen];
						fread(compData, -dataLen, 1, file);
						//TODO: Send data

					}
					fread(&dataLen, sizeof(int), 1, file);
				}

				// Filetype, ObjectType, [Components]
				// Components are not dynamic at this time.
				//reads = fread(&address, sizeof(item), instances, file);
				//if(filetype != ObjectFileType)
				//{
				//	//Debug("A non-object filetype cannot be loaded as an object");
				//	return nullptr;
				//}

				ObjectType = 0;
				fclose(file);
			}

			//Test file data
			//=================================
			/*
				filetype
				componentcount
				componentMesh  : filename
			*/
			//=================================

			//Test prefab assembly
			//=================================
			int componentCount = 1;

			//TEST CODE ONLY

			char* classes[3] = {
				"Object",
				"TestObject",
				"GameObject"
			};
			char* types[3] = {
				"Mesh",
				"Material",
				"Physical"
			};
			char* names[1] = {
				"Assets/ViveController_mesh.bin"
			};
			prefab->typeID = TypeMap::getNameId(std::string(_filename->c_str()));
			prefab->object = registeredConstructors[prefab->typeID]();
			for(int i = 0; i < 1; i++) {
				const int typeID = TypeMap::getNameId(std::string(types[i]));
				ComponentBase * component = managers[typeID]->GetComponent(names[0]);
				prefab->instantiatedComponents[typeID] = component;
				prefab->fastclone[typeID] = component->singleInstance;
				//Mesh* testing = (Mesh*)managers[typeID]->GetElement(UINT_MAX);
			}
			if(prefabID == 1) {
				const int typeIDTEMP = TypeMap::getNameId(std::string(types[2]));
				ComponentBase * componentTEMP = managers[typeIDTEMP]->GetComponent("");
				prefab->instantiatedComponents[typeIDTEMP] = componentTEMP;
				((InstantiatedCompBase *) prefab->instantiatedComponents[typeIDTEMP])->parentObject = prefab->object;
				prefab->object->SetComponent(prefab->instantiatedComponents[typeIDTEMP], typeIDTEMP);
			}

			//=================================
			//prefab->m[0] = mesh;
			//prefab->object->SetComponent<Mesh>(mesh);
			//
			prefabNames[*_filename] = prefabID;
			int x = 0;
		}
	}

	static void Instantiate(EventMessageBase *e) {
		InstantiateMessage* instantiate = (InstantiateMessage*) e;

		PrefabId pid = instantiate->GetId();
		const Object * o = prefabs[pid].object;

		//TODO: ID should be whatever
		Object* newobject = objMan->Instantiate(prefabs[pid].typeID);

		for(int i = 0; i < 64; i++) {
			if(prefabs[pid].fastclone[i]) {
				newobject->SetComponent(prefabs[pid].instantiatedComponents[i], i);
			} else if(prefabs[pid].instantiatedComponents[i] != nullptr) {
				InstantiatedCompBase* comptemp = (InstantiatedCompBase *) managers[i]->GetComponent("");
				comptemp->parentObject = newobject; // This will crash if this is not an InstantiatedCompBase
				newobject->SetComponent(comptemp, i);
			}
		}

		if(instantiate->GetReturnObject() != nullptr) {
			instantiate->SetReturnObject(newobject);
		}
		memcpy(&newobject->position.m[3], &instantiate->GetPosition(), sizeof(DirectX::XMFLOAT4));
		Mesh * test = newobject->GetComponent<Mesh>();
		MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
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
