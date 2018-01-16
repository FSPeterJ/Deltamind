#pragma once
#include "Object.h"
#include <unordered_map>
#include "MeshManager.h"

/// <summary>
/// Creates and manages prefabs loaded from the disk.
/// </summary>
class ObjectFactory {
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
	static std::unordered_map<std::string, Object*> prefabNames;

	//static std::unordered_map<int, Object*> prefabs;

	//pointer storage for prefabs, access by prefab ID
	static std::vector<Object*> prefabs;

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
	/// Creates a prefab in the prefab pool from a file if it doesn't exist already
	/// </summary>
	/// <param name="_name">name of the file to load.</param>
	static Object* CreatePrefab(std::string *_name) {
		Object * prefab = prefabNames[*_name];
		if(prefabNames[*_name]) {
			//This object prefab already exists.
		} else {
			int ObjectType = 0;
			FILE* file = nullptr;
			//int reads;
			fopen_s(&file, _name->c_str(), "rb");
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
			prefab = registeredConstructors[ObjectType]();
			prefabs.push_back(prefab);
			//
			prefabNames[*_name] = prefab;
			prefab->SetComponent<Mesh>(meshManager->GetElement(UINT_MAX));
			Mesh* temp1 = prefab->GetComponent<Mesh>();
			int x = 0;
		}
		return prefab;
	}

	/// <summary>
	/// gives an immutable pointer to the requested prefab
	/// </summary>
	/// <param name="_pid">Prefab id</param>
	static const Object *  RequestPrefab(const PrefabId _pid) {
		return prefabs[_pid];
	}

	/// <summary>
	/// Gets the count of prefabs
	/// </summary>
	static size_t GetPrefabCount() {
		return prefabs.size();
	}

	static void Shutdown()
	{
		for (auto prefab : prefabs)
		{
			delete prefab;
		}
	}
};
