#include "Renderer.h"
//#include "vld.h"
#include "InputManager.h"
#include "Messagebox.h"
#include "Game.h"
#include "ThreadPool.h"
#include "PhysicsManager.h"
#include "AnimatorManager.h"
#include "GhostTime.h"
#include "Projectile.h"
#include "EnemyBase.h"
#include "ViveController.h"
#include "Spawner.h"
#include "ObjectManager.h"
#include "MessageEvents.h"
#include "VRManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "ControllerObject.h"
#include "PhysicsTestObj.h"
#include "ProgressBar.h"
#include "AudioManager.h"
#include "BuildTool.h"
#include "EngineStructure.h"
#include "Menu.h"
#include "AStarEnemy.h"
#include "Turret.h"
#include "Player.h"

Renderer* rendInter;
Game* game;
MemoryManager MemMan;
InputManager* inputMan;
PhysicsManager* phyMan;
ObjectManager* objMan;
EngineStructure engine;
AnimatorManager* animMan;
AudioManager* audioMan;
Player* player;

GameObject* animationTest;


void ExecuteAsync() {
	Console::WriteLine << "I am executed asyncly!";
	throw std::invalid_argument("ERROR: This is a test showing we can know if a thread throws an exception on its work.\n");
}

void Setup(HINSTANCE hInstance, int nCmdShow) {
	Console::Allocate();

	Window wnd(1024, 900);

	if(!wnd.Initialize(hInstance, nCmdShow)) { Messagebox::ShowError("Error!!", "Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	_Pool_Base::RegisterMemory(&MemMan);
	Console::WriteLine << "App has been initalized!";
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

		Console::Write << e.what();
	}
	//=============================

	rendInter = new Renderer();
	audioMan = new AudioManager();
	player = new Player();
	if(player->IsVR()) {
		rendInter->Initialize(wnd, &player->transform);
		Console::WriteLine << "Renderer initialized......";
		inputMan = new InputManager(VR);
		if(inputMan) Console::WriteLine << "Input Manager initialized......";
		//audioMan->setCamera(&VRManager::GetInstance().GetPlayerPosition());
	}
	else {
		Console::WriteLine << "VR not initialized! Defaulting to 2D";
		rendInter->Initialize(wnd, &player->transform);
		Console::WriteLine << "Renderer initialized......";
		inputMan = new InputManager(KEYBOARD);
		if(inputMan) Console::WriteLine << "Input Manager initialized......";
		//audioMan->setCamera(&(rendInter->getCamera())->GetPosition());
	}
	audioMan->setCamera(&player->transform.GetMatrix());
	Console::WriteLine << "Nothing's wrong here......";

	animMan = new AnimatorManager(rendInter->getAnimationManager());
	if(animMan) Console::WriteLine << "Animation Manager initialized......";
	phyMan = new PhysicsManager();
	if(phyMan) Console::WriteLine << "Physics Manager initialized......";
	objMan = new ObjectManager(&MemMan);
	if(objMan) Console::WriteLine << "Object Manager initialized......";
	objMan->Initialize(80);
	ObjectFactory::Initialize(objMan, "NOT USED STRING");

	Console::WriteLine << "Object Factory Initialized......";
	ObjectFactory::RegisterPrefabBase<Turret>(35);
	ObjectFactory::RegisterPrefabBase<Item>(22);
	ObjectFactory::RegisterPrefabBase<ControllerObject>(20);
	ObjectFactory::RegisterPrefabBase<Gun>(20);
	ObjectFactory::RegisterPrefabBase<ProgressBar>(21);
	ObjectFactory::RegisterPrefabBase<MenuControllerItem>(23);
	ObjectFactory::RegisterPrefabBase<GameObject>(512);
	ObjectFactory::RegisterPrefabBase<Projectile>(512);
	ObjectFactory::RegisterPrefabBase<Spawner>(24);
	ObjectFactory::RegisterPrefabBase<EnemyBase>(32);
	ObjectFactory::RegisterPrefabBase<MenuCube>(5);
	ObjectFactory::RegisterPrefabBase<CoreCube>(5);
	ObjectFactory::RegisterPrefabBase<BuildTool>(24);
	ObjectFactory::RegisterPrefabBase<PhysicsTestObj>(32);
	ObjectFactory::RegisterPrefabBase<ResumeButton>(1);
	ObjectFactory::RegisterPrefabBase<RestartButton>(1);
	ObjectFactory::RegisterPrefabBase<QuitButton>(1);
	ObjectFactory::RegisterPrefabBase<PlayButton>(1);
	ObjectFactory::RegisterPrefabBase<OptionsButton>(1);
	ObjectFactory::RegisterPrefabBase<ExitButton>(1);

	ObjectFactory::RegisterPrefabBase<AStarEnemy>(10);
	Console::WriteLine << "Prefab base registered......";

	ObjectFactory::RegisterManager<Mesh, MeshManager>(rendInter->getMeshManager());
	ObjectFactory::RegisterManager<PhysicsComponent, PhysicsManager>(phyMan);
	ObjectFactory::RegisterManager<Material, MaterialManager>(rendInter->getMaterialManager());
	ObjectFactory::RegisterManager<Animator, AnimatorManager>(animMan);
	Console::WriteLine << "Managers registered......";


	//------
	// ToDo: Find an appropriate place for these?
	// You might be able to put the above stuff with it
	//=========================================================
	TypeMap::RegisterObjectAlias<ControllerObject>("ControllerObject");
	TypeMap::RegisterObjectAlias<ViveController>("ViveController");
	TypeMap::RegisterObjectAlias<MenuControllerItem>("MenuControllerItem");
	TypeMap::RegisterObjectAlias<Gun>("Gun");
	TypeMap::RegisterObjectAlias<Item>("Item");
	TypeMap::RegisterObjectAlias<ProgressBar>("ProgressBar");
	TypeMap::RegisterObjectAlias<Projectile>("Projectile");
	TypeMap::RegisterObjectAlias<Spawner>("Spawner");
	TypeMap::RegisterObjectAlias<EnemyBase>("EnemyBase");
	TypeMap::RegisterObjectAlias<MenuCube>("MenuCube");
	TypeMap::RegisterObjectAlias<CoreCube>("CoreCube");
	TypeMap::RegisterObjectAlias<GameObject>("GameObject");
	TypeMap::RegisterObjectAlias<GameObject>("ghost");
	TypeMap::RegisterObjectAlias<PhysicsTestObj>("PhysicsTestObj");
	TypeMap::RegisterObjectAlias<BuildTool>("BuildTool");
	TypeMap::RegisterObjectAlias<ResumeButton>("ResumeButton");
	TypeMap::RegisterObjectAlias<RestartButton>("RestartButton");
	TypeMap::RegisterObjectAlias<QuitButton>("QuitButton");
	TypeMap::RegisterObjectAlias<PlayButton>("PlayButton");
	TypeMap::RegisterObjectAlias<OptionsButton>("OptionsButton");
	TypeMap::RegisterObjectAlias<ExitButton>("ExitButton");

	TypeMap::RegisterObjectAlias<AStarEnemy>("AStarEnemy");
	TypeMap::RegisterObjectAlias<Turret>("Turret");

	Console::WriteLine << "Object Alias registered......";



	ObjectFactory::CreatePrefab(&std::string("Assets/DevController.ghost"), "DevController", true);
	ObjectFactory::CreatePrefab(&std::string("Assets/basicSphere.ghost"), "BasicSphere");
	ObjectFactory::CreatePrefab(&std::string("Assets/ScifiRoom.ghost"), "MainRoom");
	ObjectFactory::CreatePrefab(&std::string("Assets/ProjectileSphere.ghost"), "Projectile");
	ObjectFactory::CreatePrefab(&std::string("Assets/Spawner.ghost"), "Spawner");
	ObjectFactory::CreatePrefab(&std::string("Assets/EnemyRobot.ghost"), "TestEnemy");
	ObjectFactory::CreatePrefab(&std::string("Assets/StartCube.ghost"), "startCube");
	ObjectFactory::CreatePrefab(&std::string("Assets/CoreCube.ghost"), "CoreCube");
	ObjectFactory::CreatePrefab(&std::string("Assets/WinCube.ghost"), "WinCube");
	ObjectFactory::CreatePrefab(&std::string("Assets/LoseCube.ghost"), "LoseCube");
	ObjectFactory::CreatePrefab(&std::string("Assets/EarthMage.ghost"), "EarthMage");
	ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest1.ghost"), "PhyTest1");
	ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest2.ghost"), "PhyTest2");
	ObjectFactory::CreatePrefab(&std::string("Assets/PlaneMap1.ghost"), "Ground");
	ObjectFactory::CreatePrefab(&std::string("Assets/OverheatBar.ghost"), "GUIOverheatTest");
	ObjectFactory::CreatePrefab(&std::string("Assets/OverheatBar.ghost"), "ProgressBar");
	ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost"), "BuildTool");
	ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest3.ghost"), "PhyTest3");
	ObjectFactory::CreatePrefab(&std::string("Assets/MenuControllerItem.ghost"), "MenuController", true);
	unsigned Gun1 = ObjectFactory::CreatePrefab(&std::string("Assets/Gun.ghost"), "GunTest");
	unsigned Gun2 = ObjectFactory::CreatePrefab(&std::string("Assets/Gun2.ghost"), "GunTest2");
	ObjectFactory::CreatePrefab(&std::string("Assets/Gun.ghost"), "GunTest", true);
	ObjectFactory::CreatePrefab(&std::string("Assets/TestProjectile.ghost"), "TestProjectile");
	ObjectFactory::CreatePrefab(&std::string("Assets/AStarEnemyEdit.ghost"), "AStarEnemy");
	unsigned basicTurret = ObjectFactory::CreatePrefab(&std::string("Assets/TestTurret.ghost"), "TestTurret", true);
	ObjectFactory::CreatePrefab(&std::string("Assets/RestartButton.ghost"), "RestartButton");
	ObjectFactory::CreatePrefab(&std::string("Assets/QuitButton.ghost"), "QuitButton");


	//ObjectFactory::CreatePrefab(&std::string("Assets/TeleportSphere.ghost"));
	//ObjectFactory::CreatePrefab(&std::string("Object.ghost"));
	//ObjectFactory::CreatePrefab(&std::string("Object"));
	//ObjectFactory::CreatePrefab(&std::string("SomeCoolObject"));
	//ObjectFactory::CreatePrefab(&std::string("LeftControllerObject"));
	//ObjectFactory::CreatePrefab(&std::string("RightControllerObject"));

	Console::WriteLine << "Prefabs created......";
	//=============================

	player->LoadControllers();
	//if(VRManager::GetInstance().IsEnabled()) {
	//	VRManager::GetInstance().CreateControllers();
	//	VRManager::GetInstance().SetBuildItems({ basicTurret });
	//}
	//else {
	//	//------
	//	// Debug Controller
	//	//=========================================================
	//	ControllerObject *debugController;
	//	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 0,2,-2 }, &debugController));
	//	debugController->Init(ControllerHand::HAND_Left);
	//	debugController->SetBuildItems({ basicTurret });
	//	debugController->SetGunData(1, Gun::FireType::SEMI, 60, 50);
	//	debugController->SetGunData(2, Gun::FireType::AUTO, 8, 25);
	//}

	//GameObject* ground;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<GameObject>("Ground", {4, 0.0f, 0.0f}, (GameObject**)&ground));
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(11, { 0, 0, 0 }, nullptr));
	//GameObject* teddy;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(11, {0, 0, 0}, &teddy));
	//teddy->GetComponent<Animator>()->setState("Walk");


	////********* TEMPORARY Start Cube ************
	////TODO: Should move this to games start eventually when it is supported
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<MenuCube>("startCube", {0, 1.5f, 0.0f}, &startCube));
	//DirectX::XMStoreFloat4x4(&startCube->position, DirectX::XMLoadFloat4x4(&startCube->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	//startCube->Enable();

	////*******************************************


	//Spawner *spawner1, *spawner2;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<Spawner>( "Spawner", { 5.0f, 5.0f, 5.0f }, &spawner1));
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<Spawner>( "Spawner", { -5.0f, 5.0f, -5.0f }, &spawner2));
	//spawner2->Enable();



	//********************* PHYSICS TEST CODE **********************************
	//PhysicsTestObj *test1; //, *test2;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest1", { 0.0f, 2.0f, 1.0f }, &test1));
	////DirectX::XMStoreFloat4x4(&test1->position, DirectX::XMLoadFloat4x4(&test1->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest3", { 0.0f, 1.0f, 0.0f }, &test2));
	////DirectX::XMStoreFloat4x4(&test2->position, DirectX::XMLoadFloat4x4(&test2->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest2", { 2.0f, 2.0f, 0.0f }, &test2));
	////DirectX::XMStoreFloat4x4(&test2->position, DirectX::XMLoadFloat4x4(&test2->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest1", { -2.0f, 2.0f, 0.0f }, nullptr));

	//((PhysicsTestObj*)test1)->isControllable = true;
	//((PhysicsTestObj*)test1)->isRayCasting = true;

	//test1->Enable();




	//Turret *debugTurret;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Turret>({ 0,0,0 }, &debugTurret));
	////assert(debugTurret->GetComponent<Animator>()->setState("default"));
	////debugTurret->GetComponent<Animator>()->SetTime(3.0f);
	//debugTurret->Enable(false);

	GhostTime::Initalize();
	MessageEvents::Initilize();

	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<GameObject>("EarthMage", { 0.0f, 0.0f, 25.0f }));
	Console::WriteLine << "Starting Game Loop......";
	game = new Game();
	game->Start(player, &engine);
}

void Loop() {
	GhostTime::Tick();
	if(!game->IsPaused()) {
		phyMan->Update();
		audioMan->Update();

	}
	else {
		player->PausedUpdate();

		phyMan->PausedUpdate();
	}


	game->Update();
	inputMan->HandleInput();

	//TODO: Need a better way to do this...Maybe a paused Update delegate?
	player->leftController->Update();
	player->rightController->Update();
	
	rendInter->Render();
}

void CleanUp() {
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
	delete animMan;
	delete audioMan;
	if(game) {
		game->Clean();
		delete game;
	}
	delete player;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GHOSTBAIT));

	Setup(hInstance, nCmdShow);

	MSG msg;
	while(game->Run()) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Loop();
		}
	}

	CleanUp();

	Console::Free();

	return (int)msg.wParam;
}
