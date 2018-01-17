#pragma once
#include "Object.h"
#include <unordered_map>
#include "MeshManager.h"
#include "ObjectManager.h"

/// <summary>
/// Creates and manages prefabs loaded from the disk.
/// </summary>
class ObjectFactory {


	struct Prefab
	{
		std::function<Object*(void)> prefabBaseconstructor;
		std::function<Object*(void)> genericComponents;
		std::vector<ComponentBase*> instantiatedComponents;
		Object* object;
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

	//map Names to prefabs
	static std::unordered_map<std::string, Prefab*> prefabNames;

	//static std::unordered_map<int, Object*> prefabs;
	//pointer storage for prefabs, access by Prefab ID
	static std::vector<Prefab*> prefabs;

	// Managers
	static MeshManager * meshManager;
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
	static void Initialize(MeshManager * _meshManager) {
		int r = TypeMap<ComponentBase>::getTypeId<std::result_of<decltype(&MeshManager::GetElement)(int&)>>();
		MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
		meshManager = _meshManager;
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
			prefab->object = registeredConstructors[ObjectType]();
			prefabs.push_back(prefab);
			//
			prefabNames[*_filename] = prefab;
			prefab->object->SetComponent<Mesh>(meshManager->GetElement(UINT_MAX));
			int x = 0;
		}
	}


	static void Instantiate(EventMessageBase *e) {
		InstantiateMessage* instantiate = (InstantiateMessage*)e;

		PrefabId pid = instantiate->GetId();
		const Object * o = prefabs[pid]->object;


		Object* newobject = ObjectManager::Instantiate(o);
		//for (auto element : prefabs[pid]->genericComponents)
		//{
		//	
		//}

		if(instantiate->GetReturnObject() != nullptr)
		{

			instantiate->SetReturnObject(newobject);
		}

		newobject->position.r[3] = XMLoadFloat4(&instantiate->GetPosition());
		MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
	}


	/// <summary>
	/// gives an immutable pointer to the requested Prefab
	/// </summary>
	/// <param name="_pid">Prefab id</param>
	static const Object *  RequestPrefab(const PrefabId _pid) {
		return prefabs[_pid]->object;
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
			delete prefab;
		}
	}
};
