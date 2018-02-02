#pragma once
#include <vector>
#include <unordered_map>
#include <bitset>
#include "Object.h"
#include "functional"       // for function
#include "StdHeader.h"      // for PrefabId

class ObjectManager;
class GameObject;
class IComponentManager;
class EventMessageBase;
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
	static GameObject* ConstructorFunc() {
		return new T;
	}

	ObjectFactory() {};

	static std::unordered_map<unsigned, std::function<Object*(void)>> registeredConstructors;

	static std::vector<IComponentManager*> managers;

	//static std::unordered_map<std::string, IComponentManager*> managerNames;

	//map Names to prefabs
	static std::unordered_map<std::string, unsigned> prefabNames;
	static std::unordered_map<unsigned, unsigned> Object2Prefab;

	//static std::unordered_map<int,GameObject*> prefabs;
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
	static void Initialize(ObjectManager* _objMan);

	~ObjectFactory() {};

	/// <summary>
	/// Registeres the constructor of a given object and it's object type ID (class) in the factory
	/// </summary>
	/// <param name="_id">Class ID to register.</param>
	/// <param name="ioClassname">String stored in Ghostbait files that determine which class the data is supposed to go in.
	///	Changing this value means you MUST change this in every ghostbait file.
	/// </param>
	template <typename ObjectType>
	static void RegisterPrefabBase(unsigned size) {
		registeredConstructors[TypeMap::GetObjectTypeID<ObjectType>()] = &ConstructorFunc<ObjectType>;
		objMan->CreatePool<ObjectType>(size);
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

	static void CreatePrefab(std::string *_filename, char* DEBUG_STRING_NAME = nullptr, bool objectPrefabOverride = false);

	static void Instantiate(EventMessageBase *e);

	static void InstantiateByType(EventMessageBase *e);

	static void InstantiateByName(EventMessageBase *e);

	static GameObject* ActivateObject(PrefabId pid);

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
