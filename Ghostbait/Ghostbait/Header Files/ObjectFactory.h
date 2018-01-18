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


	struct Prefab
	{
		ComponentBase* instantiatedComponents[64];
		int managers[64];
		Object* object;
		std::bitset<64> fastclone;
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

	static std::vector<ManagerInterface*> managers;

	//static std::unordered_map<std::string, ManagerInterface*> managerNames;

	//map Names to prefabs
	static std::unordered_map<std::string, Prefab*> prefabNames;

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
	static void Initialize() {
		int r = TypeMap::getTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
		MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
	}

	~ObjectFactory() {};

	/// <summary>
	/// Registeres the constructor of a given object and it's object type ID (class) in the factory
	/// </summary>
	/// <param name="_id">Class ID to register.</param>
	template <typename T>
	static void RegisterPrefabBase(const int _id = 0) {
		registeredConstructors[_id] = &ConstructorFunc<T>;
	}

	template <typename ComponentType, typename ManagerType>
	static void RegisterManager(ManagerInterface * manager) {
		//Mess
		const int tid = TypeMap::getTypeId<ComponentType>();
		if(managers.size() <= tid)
		{
			managers.resize(tid + 1);
		}
		managers[tid] = manager;

	}


	/// <summary>
	/// Creates a Prefab in the Prefab pool from a file if it doesn't exist already
	/// </summary>
	/// <param name="_filename">name of the file to load.</param>
	static void CreatePrefab(std::string *_filename) {
		Prefab * prefab = prefabNames[*_filename];
		if(prefabNames[*_filename]) {
			//This Prefab already exists.
		}
		else {
			int ObjectType = 0;
			FILE* file = nullptr;
			//int reads;
			fopen_s(&file, _filename->c_str(), "rb");
			if(file) {
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
			prefab = new Prefab;
			prefab->object = registeredConstructors[ObjectType]();
			int componentCount = 1;

			//TEST CODE ONLY

			char* types[2] = {
				"Mesh",
				""
			};
			int names[1] = {
				UINT_MAX
			};
			for(int i = 0; i < 1; i++)
			{
				int typeID = TypeMap::getNameId(std::string(types[i]));
				ComponentBase * component = managers[typeID]->GetElement(names[0]);
				prefab->instantiatedComponents[typeID] = component;
				prefab->managers[typeID] = 0;
				prefab->fastclone[typeID] = component->singleInstance;
				Mesh* testing = (Mesh*)managers[typeID]->GetElement(UINT_MAX);


			}

			//=================================

			//prefab->m[0] = mesh;

			//prefab->object->SetComponent<Mesh>(mesh);
			prefabs.push_back(*prefab);
			//
			prefabNames[*_filename] = prefab;
			int x = 0;
		}
	}


	static void Instantiate(EventMessageBase *e) {
		InstantiateMessage* instantiate = (InstantiateMessage*)e;

		PrefabId pid = instantiate->GetId();
		const Object * o = prefabs[pid].object;


		Object* newobject = ObjectManager::Instantiate(o);

		for(int i = 0; i < 64; i++)
		{
			if(prefabs[pid].fastclone[i])
			{
				;
				newobject->SetComponent(prefabs[pid].instantiatedComponents[i], i);
			};
		}

		if(instantiate->GetReturnObject() != nullptr)
		{
			instantiate->SetReturnObject(newobject);
		}

		newobject->position.r[3] = XMLoadFloat4(&instantiate->GetPosition());
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

	static void Shutdown()
	{
		for(auto prefab : prefabs)
		{
			delete prefab.object;
		}
	}
};


