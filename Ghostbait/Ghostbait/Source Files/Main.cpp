#include <windows.h>
#include "vld.h" //Before commenting this out, please see TODO below and then don't comment this out...
#include "Resource.h"
#include "Window.h"
#include "Console.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "MemoryManager.h"
//#include "GameObject.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "GameObjectComponent.h"
#include "EngineStructure.h"
#include "VRManager.h"
#include "InputManager.h"
#include "Messagebox.h"
#include "Game.h"
#include "ThreadPool.h"
#include "PhysicsManager.h"
#include "GhostTime.h"
#include "Projectile.h"
#include "EnemyBase.h"
#include "ViveController.h"
#include "ControllerObject.h"
#include "Spawner.h"

Renderer* rendInter;
VRManager* vrMan;
Game* game;
InputManager* inputMan;
PhysicsManager* phyMan;
MemoryManager MemMan;
ObjectManager* objMan;
EngineStructure engine;



void ExecuteAsync() {
	WriteLine("I am executed asyncly!");
	throw std::invalid_argument("ERROR: This is a test showing we can know if a thread throws an exception on it's work.\n");
}

void Setup(HINSTANCE hInstance, int nCmdShow) {
	Window wnd(900, 900);

	if(!wnd.Initialize(hInstance, nCmdShow)) { Messagebox::ShowError("Error!!", "Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	ConsoleAllocate();
	_Pool_Base::RegisterMemory(&MemMan);
	WriteLine("App has been initalized!");

	//Minimize();

#pragma region testing
	//EngineStructure engine;
	//SomeCoolObject* t = new SomeCoolObject();
	//engine.ExecuteAwake();
	//engine.ExecuteUpdate();
	//delete t;
	//MessageEvents::SendMessage(EVENT_Input, InputMessage(teleport, 0.567f));
	//system("pause");
	//CleanUp();
	//Free();
	//exit(0);
#pragma endregion

	//Memory Test
	//=============================
	//WriteLine((int)sizeof(Pool<Object>(15)));
	//WriteLine((int)sizeof(Pool<SomeLeakyObject>(15)));

	//=============================
	//Multithreading Test
	//=============================

	ThreadPool::Start();
	auto temp = ThreadPool::MakeJob(ExecuteAsync);

	// check future for errors and / or completion
	// This is a proof of concept, thread decoupling with .get is still uncertain.
	try {
		temp.get();
	}
	catch(const std::exception& e) {
		//std::rethrow_exception(e);
		// handle it

		std::cout << e.what();
	}
	//=============================

	vrMan = new VRManager();
	rendInter = new Renderer();
	bool isVR = vrMan->Init();
	if(isVR) {
		rendInter->Initialize(wnd, vrMan);
		inputMan = new InputManager(VR, vrMan);
	}
	else {
		WriteLine("VR not initialized! Defaulting to 2D");
		rendInter->Initialize(wnd, nullptr);
		inputMan = new InputManager(KEYBOARD);
	}

	phyMan = new PhysicsManager();
	objMan = new ObjectManager(&MemMan);
	objMan->Initialize(80);

	ObjectFactory::Initialize(objMan);
	ObjectFactory::RegisterPrefabBase<ControllerObject>(8);
	ObjectFactory::RegisterPrefabBase<Gun>(10);
	ObjectFactory::RegisterPrefabBase<ViveController>(8);
	ObjectFactory::RegisterPrefabBase<GameObject>(512);
	ObjectFactory::RegisterPrefabBase<Projectile>(512);
	ObjectFactory::RegisterPrefabBase<Spawner>(16);
	ObjectFactory::RegisterPrefabBase<EnemyBase>(32);
	ObjectFactory::RegisterPrefabBase<MenuCube>(5);
	ObjectFactory::RegisterPrefabBase<CoreCube>(5);
	ObjectFactory::RegisterManager<Mesh, MeshManager>(rendInter->getMeshManager());
	ObjectFactory::RegisterManager<PhysicsComponent, PhysicsManager>(phyMan);
	ObjectFactory::RegisterManager<Material, MaterialManager>(rendInter->getMaterialManager());

	//------
	// Scenemanager would make this
	//=========================================================
	TypeMap::RegisterObjectAlias<ControllerObject>("ControllerObject");
	TypeMap::RegisterObjectAlias<ViveController>("ViveController");
	TypeMap::RegisterObjectAlias<Gun>("Gun");
	TypeMap::RegisterObjectAlias<Projectile>("Projectile");
	TypeMap::RegisterObjectAlias<Spawner>("Spawner");
	TypeMap::RegisterObjectAlias<EnemyBase>("EnemyBase");
	TypeMap::RegisterObjectAlias<MenuCube>("MenuCube");
	TypeMap::RegisterObjectAlias<CoreCube>("CoreCube");
	TypeMap::RegisterObjectAlias<GameObject>("GameObject");

	ObjectFactory::CreatePrefab(&std::string("Assets/EmptyContainer2.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/ViveController2.ghost"), true);
	ObjectFactory::CreatePrefab(&std::string("Assets/basicSphere.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/ScifiRoom.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/ProjectileSphere.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/Spawner.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/EnemyRobot.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/StartCube.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/CoreCube.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/WinCube.ghost"));
	ObjectFactory::CreatePrefab(&std::string("Assets/LoseCube.ghost"));

	//ObjectFactory::CreatePrefab(&std::string("Object.ghost"));
	//ObjectFactory::CreatePrefab(&std::string("Object"));
	//ObjectFactory::CreatePrefab(&std::string("SomeCoolObject"));
	//ObjectFactory::CreatePrefab(&std::string("LeftControllerObject"));
	//ObjectFactory::CreatePrefab(&std::string("RightControllerObject"));
	//=============================

	game = new Game();
	game->Start();
	if(isVR) vrMan->CreateControllers();
	DirectX::XMFLOAT4X4 roomMatrix;
	DirectX::XMStoreFloat4x4(&roomMatrix, DirectX::XMMatrixScaling(0.15f, 0.15f, 0.15f) * DirectX::XMMatrixTranslation(0, 3, 0));
	MenuCube* startCube;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(3, roomMatrix));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(7, {0, 1.5f, 0.0f}, (Object**)&startCube));
	DirectX::XMStoreFloat4x4(&startCube->position, DirectX::XMLoadFloat4x4(&startCube->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	startCube->Enable();

	//	Object* cube1, *cube2;

	//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(2, { 0,1,0 }, (Object**)&gunthing));
	//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(4, { 0,1,4 }, (Object**)&cube2));


	

	//Object* cube1 = Object::Create<Object>({0,-1,0,1}, 1);
	//SomeCoolObject* cube2 = Object::Create<SomeCoolObject>({0,-3,0,1}, 2);

	//cube1->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(0.5f, -1.0f, 0.0f);
	//cube2->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(1.0f, 0.0f, 0.0f);

	//MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(cube1));

	GhostTime::Initalize();
}

void Loop() {
	phyMan->Update();
	inputMan->HandleInput();
	engine.ExecuteUpdate();
	engine.ExecuteLateUpdate();
	rendInter->Render();
}

void CleanUp() {
	if(vrMan) {
		delete vrMan;
	}
	if(rendInter) {
		rendInter->Destroy();
		delete rendInter;
	}
	ObjectFactory::Shutdown();
	ThreadPool::Shutdown();
	objMan->Shutdown();
	delete objMan;
	delete phyMan;
	delete inputMan;
	if(game) {
		game->Clean();
		delete game;
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GHOSTBAIT));

	Setup(hInstance, nCmdShow);

	MSG msg;
	while(true) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//test2->position = vrMan->hmdPose;
			//test3->position = XMMatrixTranspose(XMLoadFloat4x4(&(rendInter->leftEye.camera.view)));
			//test4->position = XMMatrixTranspose(XMLoadFloat4x4(&(rendInter->rightEye.camera.view)));

			Loop();
		}
	}

	CleanUp();

	Free();

	return (int)msg.wParam;
}
