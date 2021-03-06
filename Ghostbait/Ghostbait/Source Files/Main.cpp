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
#include "DStarEnemy.h"
#include "MTDSLEnemy.h"
#include "Turret.h"
#include "Player.h"
#include "Logger.h"
#include "Core.h"
#include "Ground.h"
#include "Monitor.h"
#include "ScrollingUVManager.h"
#include "DisplayBoard.h"
#include "ControllerPillar.h"
#include "PDA.h"
#include "OptionsManager.h"
#include "ForceField.h"
#include "TargetEnemy.h"
#include "CoreShield.h"
#include "Emitter.h"
#include "Door.h"
#include "DMLogo.h"
#include "SSLogo.h"
#include "Wall.h"
#include "Powerup.h"

using namespace Threadding;

const bool FULLSCREEN = 1;

Renderer* rendInter;
Game* game;
MemoryManager MemMan;
InputManager* inputMan;
PhysicsManager* phyMan;
ObjectManager* objMan;
EngineStructure engine;
AnimatorManager* animMan;
AudioManager* audioMan;
ScrollingUVManager* scrollMan;
Player* player;


//void ExecuteAsync() {
//	Console::WriteLine << "I am executed asyncly!";
//	throw std::invalid_argument("ERROR: This is a test showing we can know if a thread throws an exception on its work.\n");
//}
Window wnd(1200, 900);
void Setup(HINSTANCE hInstance, int nCmdShow) {
	//throw std::runtime_error("Nothing");

	ThreadPool::Start();
	Console::Allocate();
	//wnd =Window(1200, 900);
	if(!wnd.Initialize(hInstance, FULLSCREEN ? SW_MAXIMIZE : nCmdShow)) { Messagebox::ShowError("Error!!", "Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	_Pool_Base::RegisterMemory(&MemMan);
	Console::WriteLine << "App has been initalized!";

	//=============================
	//Multithreading Test
	//=============================

	//auto temp = ThreadPool::MakeJob(ExecuteAsync);

	//// check future for errors and / or completion
	//// This is a proof of concept, thread decoupling with .get is still uncertain.
	//try {
	//	temp.get();
	//}
	//catch(const std::exception& e) {
	//	//std::rethrow_exception(e);
	//	// handle it

	//	Console::Write << e.what();
	//}
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
		//audioMan->setCamera(&(rendInter->GetCamera())->GetPosition());
	}
	audioMan->setCamera(&player->transform.GetMatrix());
	Console::WriteLine << "Nothing's wrong here......";

	scrollMan = new ScrollingUVManager();
	if(scrollMan) Console::WriteLine << "Scrolling UV Manager initialized......";
	animMan = new AnimatorManager(rendInter->getAnimationManager());
	if(animMan) Console::WriteLine << "Animation Manager initialized......";
	phyMan = new PhysicsManager();
	if(phyMan) Console::WriteLine << "Physics Manager initialized......";
	objMan = new ObjectManager(&MemMan);
	if(objMan) Console::WriteLine << "Object Manager initialized......";
	objMan->Initialize(80);
	ObjectFactory::Initialize(objMan, "NOT USED STRING");
	Console::WriteLine << "Object Factory Initialized......";



	//This is absurd.  Will move soon.

	ObjectFactory::RegisterPrefabBase<Turret_Long>(102);
	ObjectFactory::RegisterPrefabBase<Turret_Medium>(102);
	ObjectFactory::RegisterPrefabBase<Turret_Short>(102);

	ObjectFactory::RegisterPrefabBase<Item>(16);
	ObjectFactory::RegisterPrefabBase<ControllerObject>(2);
	ObjectFactory::RegisterPrefabBase<ViveController>(20);
	ObjectFactory::RegisterPrefabBase<SMG>(4);
	ObjectFactory::RegisterPrefabBase<Pistol>(4);
	ObjectFactory::RegisterPrefabBase<PDA>(5);
	ObjectFactory::RegisterPrefabBase<ProgressBar>(8);
	ObjectFactory::RegisterPrefabBase<MenuControllerItem>(2);
	ObjectFactory::RegisterPrefabBase<GameObject>(512);
	ObjectFactory::RegisterPrefabBase<Projectile>(512);
	ObjectFactory::RegisterPrefabBase<SpawnerObject>(6);
	ObjectFactory::RegisterPrefabBase<EnemyBase>(300);
	ObjectFactory::RegisterPrefabBase<CoreShield>(1);
	ObjectFactory::RegisterPrefabBase<Core>(1);
	ObjectFactory::RegisterPrefabBase<BuildTool>(24);
	ObjectFactory::RegisterPrefabBase<PhysicsTestObj>(32);
	ObjectFactory::RegisterPrefabBase<ResumeButton>(1);
	ObjectFactory::RegisterPrefabBase<RestartButton>(1);
	ObjectFactory::RegisterPrefabBase<QuitButton>(1);
	ObjectFactory::RegisterPrefabBase<PlayButton>(1);
	ObjectFactory::RegisterPrefabBase<OptionsButton>(1);
	ObjectFactory::RegisterPrefabBase<ExitButton>(1);
	ObjectFactory::RegisterPrefabBase<BackButton>(12);
	ObjectFactory::RegisterPrefabBase<EasyButton>(1);
	ObjectFactory::RegisterPrefabBase<MediumButton>(1);
	ObjectFactory::RegisterPrefabBase<HardButton>(1);
	ObjectFactory::RegisterPrefabBase<TutorialButton>(1);
	ObjectFactory::RegisterPrefabBase<CreditsButton>(1);
	ObjectFactory::RegisterPrefabBase<MasterUpButton>(1);
	ObjectFactory::RegisterPrefabBase<MasterDownButton>(1);
	ObjectFactory::RegisterPrefabBase<MusicUpButton>(1);
	ObjectFactory::RegisterPrefabBase<MusicDownButton>(1);
	ObjectFactory::RegisterPrefabBase<SFXUpButton>(1);
	ObjectFactory::RegisterPrefabBase<SFXDownButton>(1);
	ObjectFactory::RegisterPrefabBase<BrightnessUpButton>(1);
	ObjectFactory::RegisterPrefabBase<BrightnessDownButton>(1);
	ObjectFactory::RegisterPrefabBase<MouseSensitivityUpButton>(1);
	ObjectFactory::RegisterPrefabBase<MouseSensitivityDownButton>(1);
	ObjectFactory::RegisterPrefabBase<AcceptOptionsButton>(1);
	ObjectFactory::RegisterPrefabBase<CancelOptionsButton>(1);
	ObjectFactory::RegisterPrefabBase<NextButton>(1);
	ObjectFactory::RegisterPrefabBase<SkipButton>(1);
	ObjectFactory::RegisterPrefabBase<RevertOptionsButton>(1);
	ObjectFactory::RegisterPrefabBase<QuitConfirmButton>(1);
	ObjectFactory::RegisterPrefabBase<QuitCancelButton>(1);
	ObjectFactory::RegisterPrefabBase<Label>(5);
	ObjectFactory::RegisterPrefabBase<MasterValue>(5);
	ObjectFactory::RegisterPrefabBase<MusicValue>(1);
	ObjectFactory::RegisterPrefabBase<SFXValue>(1);
	ObjectFactory::RegisterPrefabBase<BrightnessValue>(1);
	ObjectFactory::RegisterPrefabBase<MouseSensitivityValue>(1);
	ObjectFactory::RegisterPrefabBase<ForceField>(6);
	ObjectFactory::RegisterPrefabBase<DMLogo>(1);
	ObjectFactory::RegisterPrefabBase<SSLogo>(1);
	ObjectFactory::RegisterPrefabBase<Door>(2);
	ObjectFactory::RegisterPrefabBase<Ground>(4);
	ObjectFactory::RegisterPrefabBase<Monitor>(2);
	ObjectFactory::RegisterPrefabBase<DisplayBoard>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Move>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Pause>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Inventory>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Items>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Build>(1);
	ObjectFactory::RegisterPrefabBase<DisplayBoard_Shoot>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar_Move>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar_Shoot>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar_Pause>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar_Switch>(1);
	ObjectFactory::RegisterPrefabBase<ControllerPillar_Build>(1);
	ObjectFactory::RegisterPrefabBase<AStarEnemy>(300);
	ObjectFactory::RegisterPrefabBase<DStarEnemy>(10);
	ObjectFactory::RegisterPrefabBase<MTDSLEnemy>(300);
	ObjectFactory::RegisterPrefabBase<TargetEnemy>(6);
	ObjectFactory::RegisterPrefabBase<Wall>(100);
	ObjectFactory::RegisterPrefabBase<Powerup>(10 /*TODO: Make this bigger or have hard set cap?*/);
	Console::WriteLine << "Prefab base registered......";

	ObjectFactory::RegisterManager<Mesh, MeshManager>(rendInter->GetMeshManager());
	ObjectFactory::RegisterManager<PhysicsComponent, PhysicsManager>(phyMan);
	ObjectFactory::RegisterManager<Material, MaterialManager>(rendInter->GetMaterialManager());
	ObjectFactory::RegisterManager<Emitter, ParticleManager>(rendInter->GetParticleManager());
	ObjectFactory::RegisterManager<Animator, AnimatorManager>(animMan);
	ObjectFactory::RegisterManager<ScrollingUV, ScrollingUVManager>(scrollMan);
	Console::WriteLine << "Managers registered......";

	//------
	// ToDo: Find an appropriate place for these?
	// You might be able to put the above stuff with it
	//=========================================================
	TypeMap::RegisterObjectAlias<ControllerObject>("ControllerObject");
	TypeMap::RegisterObjectAlias<ViveController>("ViveController");
	TypeMap::RegisterObjectAlias<MenuControllerItem>("MenuControllerItem");
	TypeMap::RegisterObjectAlias<SMG>("SMG");
	TypeMap::RegisterObjectAlias<Pistol>("Pistol");
	TypeMap::RegisterObjectAlias<Item>("Item");
	TypeMap::RegisterObjectAlias<ProgressBar>("ProgressBar");
	TypeMap::RegisterObjectAlias<Projectile>("Projectile");
	TypeMap::RegisterObjectAlias<SpawnerObject>("Spawner");
	TypeMap::RegisterObjectAlias<EnemyBase>("EnemyBase");
	TypeMap::RegisterObjectAlias<CoreShield>("CoreShield");
	TypeMap::RegisterObjectAlias<Core>("Core");
	TypeMap::RegisterObjectAlias<GameObject>("GameObject");
	TypeMap::RegisterObjectAlias<GameObject>("ghost");
	TypeMap::RegisterObjectAlias<PhysicsTestObj>("PhysicsTestObj");
	TypeMap::RegisterObjectAlias<BuildTool>("BuildTool");
	TypeMap::RegisterObjectAlias<Label>("Label");
	TypeMap::RegisterObjectAlias<DMLogo>("DMLogo");
	TypeMap::RegisterObjectAlias<SSLogo>("SSLogo");

	TypeMap::RegisterObjectAlias<ResumeButton>("ResumeButton");
	TypeMap::RegisterObjectAlias<RestartButton>("RestartButton");
	TypeMap::RegisterObjectAlias<QuitButton>("QuitButton");
	TypeMap::RegisterObjectAlias<PlayButton>("PlayButton");
	TypeMap::RegisterObjectAlias<OptionsButton>("OptionsButton");
	TypeMap::RegisterObjectAlias<ExitButton>("ExitButton");
	TypeMap::RegisterObjectAlias<BackButton>("BackButton");
	TypeMap::RegisterObjectAlias<EasyButton>("EasyButton");
	TypeMap::RegisterObjectAlias<MediumButton>("MediumButton");
	TypeMap::RegisterObjectAlias<HardButton>("HardButton");
	TypeMap::RegisterObjectAlias<CreditsButton>("CreditsButton");
	TypeMap::RegisterObjectAlias<TutorialButton>("TutorialButton");
	TypeMap::RegisterObjectAlias<MasterUpButton>("MasterUpButton");
	TypeMap::RegisterObjectAlias<MasterDownButton>("MasterDownButton");
	TypeMap::RegisterObjectAlias<MusicUpButton>("MusicUpButton");
	TypeMap::RegisterObjectAlias<MusicDownButton>("MusicDownButton");
	TypeMap::RegisterObjectAlias<SFXUpButton>("SFXUpButton");
	TypeMap::RegisterObjectAlias<SFXDownButton>("SFXDownButton");
	TypeMap::RegisterObjectAlias<BrightnessUpButton>("BrightnessUpButton");
	TypeMap::RegisterObjectAlias<BrightnessDownButton>("BrightnessDownButton");
	TypeMap::RegisterObjectAlias<MouseSensitivityUpButton>("MouseSensitivityUpButton");
	TypeMap::RegisterObjectAlias<MouseSensitivityDownButton>("MouseSensitivityDownButton");
	TypeMap::RegisterObjectAlias<AcceptOptionsButton>("AcceptOptionsButton");
	TypeMap::RegisterObjectAlias<CancelOptionsButton>("CancelOptionsButton");
	TypeMap::RegisterObjectAlias<NextButton>("NextButton");
	TypeMap::RegisterObjectAlias<SkipButton>("SkipButton");
	TypeMap::RegisterObjectAlias<RevertOptionsButton>("RevertOptionsButton");
	TypeMap::RegisterObjectAlias<QuitConfirmButton>("QuitConfirmButton");
	TypeMap::RegisterObjectAlias<QuitCancelButton>("QuitCancelButton");
	TypeMap::RegisterObjectAlias<MasterValue>("MasterValue");
	TypeMap::RegisterObjectAlias<MusicValue>("MusicValue");
	TypeMap::RegisterObjectAlias<SFXValue>("SFXValue");
	TypeMap::RegisterObjectAlias<BrightnessValue>("BrightnessValue");
	TypeMap::RegisterObjectAlias<MouseSensitivityValue>("MouseSensitivityValue");



	TypeMap::RegisterObjectAlias<ForceField>("ForceField");
	TypeMap::RegisterObjectAlias<Ground>("Ground");
	TypeMap::RegisterObjectAlias<PDA>("PDA");
	TypeMap::RegisterObjectAlias<Monitor>("Monitor");
	TypeMap::RegisterObjectAlias<Door>("Door");
	TypeMap::RegisterObjectAlias<Wall>("Wall");
	TypeMap::RegisterObjectAlias<Powerup>("Powerup");

	TypeMap::RegisterObjectAlias<DisplayBoard>("DisplayBoard");
	TypeMap::RegisterObjectAlias<DisplayBoard_Move>("DisplayBoard_Move");
	TypeMap::RegisterObjectAlias<DisplayBoard_Pause>("DisplayBoard_Pause");
	TypeMap::RegisterObjectAlias<DisplayBoard_Inventory>("DisplayBoard_Inventory");
	TypeMap::RegisterObjectAlias<DisplayBoard_Items>("DisplayBoard_Items");
	TypeMap::RegisterObjectAlias<DisplayBoard_Build>("DisplayBoard_Build");
	TypeMap::RegisterObjectAlias<DisplayBoard_Shoot>("DisplayBoard_Shoot");

	TypeMap::RegisterObjectAlias<ControllerPillar>("ControllerPillar");
	TypeMap::RegisterObjectAlias<ControllerPillar_Move>("ControllerPillar_Move");
	TypeMap::RegisterObjectAlias<ControllerPillar_Shoot>("ControllerPillar_Shoot");
	TypeMap::RegisterObjectAlias<ControllerPillar_Pause>("ControllerPillar_Pause");
	TypeMap::RegisterObjectAlias<ControllerPillar_Switch>("ControllerPillar_Switch");
	TypeMap::RegisterObjectAlias<ControllerPillar_Build>("ControllerPillar_Build");

	TypeMap::RegisterObjectAlias<AStarEnemy>("AStarEnemy");
	TypeMap::RegisterObjectAlias<DStarEnemy>("DStarEnemy");
	TypeMap::RegisterObjectAlias<Turret_Long>("Turret_Long");
	TypeMap::RegisterObjectAlias<Turret_Medium>("Turret_Medium");
	TypeMap::RegisterObjectAlias<Turret_Short>("Turret_Short");

	TypeMap::RegisterObjectAlias<MTDSLEnemy>("MTDSLEnemy");
	TypeMap::RegisterObjectAlias<TargetEnemy>("TargetEnemy");


	Console::WriteLine << "Object Alias registered......";



	ObjectFactory::CreatePrefab(&std::string("Assets/DevController.ghost"), "DevController", true);
	//ObjectFactory::CreatePrefab(&std::string("Assets/basicSphere.ghost"), "BasicSphere");
	//ObjectFactory::CreatePrefab(&std::string("Assets/ScifiRoom.ghost"), "MainRoom");
	//ObjectFactory::CreatePrefab(&std::string("Assets/Laser.ghost"), "Projectile");
	//ObjectFactory::CreatePrefab(&std::string("Assets/Spawner.ghost"), "Spawner");
	//ObjectFactory::CreatePrefab(&std::string("Assets/EnemyRobot.ghost"), "TestEnemy");
//	ObjectFactory::CreatePrefab(&std::string("Assets/StartCube.ghost"), "startCube");
	//ObjectFactory::CreatePrefab(&std::string("Assets/Core.ghost"), "Core");
	//ObjectFactory::CreatePrefab(&std::string("Assets/WinCube.ghost"), "WinCube");
	//ObjectFactory::CreatePrefab(&std::string("Assets/LoseCube.ghost"), "LoseCube");
	//ObjectFactory::CreatePrefab(&std::string("Assets/EarthMage.ghost"), "EarthMage");
	//ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest1.ghost"), "PhyTest1");
	//ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest2.ghost"), "PhyTest2");
	//ObjectFactory::CreatePrefab(&std::string("Assets/PlaneMap1.ghost"), "Ground");
	//ObjectFactory::CreatePrefab(&std::string("Assets/OverheatBar.ghost"), "GUIOverheatTest");
	//ObjectFactory::CreatePrefab(&std::string("Assets/OverheatBar.ghost"), "ProgressBar");
	//ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost"), "BuildTool");
	//ObjectFactory::CreatePrefab(&std::string("Assets/PhysicsTest3.ghost"), "PhyTest3");
	ObjectFactory::CreatePrefab(&std::string("Assets/MenuControllerItem.ghost"), "MenuController", true);
	//unsigned Gun1 = ObjectFactory::CreatePrefab(&std::string("Assets/Pistol.ghost"), "Pistol", true);
	//unsigned Gun2 = ObjectFactory::CreatePrefab(&std::string("Assets/smg.ghost"), "smg");
	//ObjectFactory::CreatePrefab(&std::string("Assets/Gun.ghost"), "GunTest", true);
	//ObjectFactory::CreatePrefab(&std::string("Assets/TestProjectile.ghost"), "TestProjectile");
	//ObjectFactory::CreatePrefab(&std::string("Assets/AStarEnemyEdit.ghost"), "AStarEnemy");
	ObjectFactory::CreatePrefab(&std::string("Assets/AStarEnemyEditEdit.ghost"), "AStarEnemy");
	//ObjectFactory::CreatePrefab(&std::string("Assets/DStarEnemyEdit.ghost"), "DStarEnemy");
	//ObjectFactory::CreatePrefab(&std::string("Assets/ResumeButton.ghost"), "ResumeButton");
	//unsigned basicTurret = ObjectFactory::CreatePrefab(&std::string("Assets/TestTurret.ghost"), "TestTurret", true);
	//ObjectFactory::CreatePrefab(&std::string("Assets/RestartButton.ghost"), "RestartButton");
	//ObjectFactory::CreatePrefab(&std::string("Assets/QuitButton.ghost"), "QuitButton");
	//ObjectFactory::CreatePrefab(&std::string("Assets/MTDSLEnemy.ghost"), "MTDSLEnemy");

	//ObjectFactory::CreatePrefab(&std::string("Assets/TransparencyTest.ghost"), "Ttest");

	Console::WriteLine << "Prefabs created......";
	//=============================

	player->InitControllers();


	//GameObject* teddy;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(11, {0, 0, 0}, &teddy));
	//teddy->GetComponent<Animator>()->setState("Walk");

	//********************* PHYSICS TEST CODE **********************************
	//PhysicsTestObj *test1, *test2;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest1", { -1.0f, 2.0f, 1.0f }, &test1));
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest1", { 3.0f, 2.0f, 1.0f }, &test2));
	////DirectX::XMStoreFloat4x4(&test1->position, DirectX::XMLoadFloat4x4(&test1->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest3", { 0.0f, 1.0f, 0.0f }, &test2));
	////DirectX::XMStoreFloat4x4(&test2->position, DirectX::XMLoadFloat4x4(&test2->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest2", { 2.0f, 2.0f, 0.0f }, &test2));
	////DirectX::XMStoreFloat4x4(&test2->position, DirectX::XMLoadFloat4x4(&test2->position) * DirectX::XMMatrixRotationRollPitchYaw(0.5f, 0.5f, 0.5f));
	////MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<PhysicsTestObj>("PhyTest1", { -2.0f, 2.0f, 0.0f }, nullptr));
	//
	//((PhysicsTestObj*)test1)->isControllable = true;
	//((PhysicsTestObj*)test1)->isRayCasting = true;
	//test1->PersistOnReset();
	//test1->AnExtremelyBadTemporaryFunction(rendInter->GetParticleManager(), rendInter->GetMaterialManager());
	//test1->Enable();
	//((PhysicsTestObj*)test2)->isControllable = true;
	//((PhysicsTestObj*)test2)->isRayCasting = true;
	//test2->PersistOnReset();
	//test2->AnExtremelyBadTemporaryFunction(rendInter->GetParticleManager(), rendInter->GetMaterialManager());
	//test2->Enable();




	GhostTime::Initalize();
	MessageEvents::Initilize();
	audioMan->LoadBanks();
	OptionsManager::GetInstance().Initialize(rendInter, audioMan, player, "Saves//Settings.ghost");
	Console::WriteLine << "Starting Game Loop......";
	game = new Game();
	game->Start(player, &engine, rendInter->getHud());
	//game->Start(player, &engine, "level0", "Level Files//level0_hard.xml");
}

void Loop() {
	GhostTime::Tick();
	//if(!game->IsPaused()) {
	//	ThreadPool::CreateMemberJob<void>(phyMan, &PhysicsManager::Update);
	//	ThreadPool::CreateMemberJob<void>(audioMan, &AudioManager::Update);
	//} else {
	//	player->PausedUpdate();
	//	phyMan->PausedUpdate();
	//}
	//ThreadPool::CreateMemberJob<void>(inputMan, &InputManager::HandleInput);

	//ThreadPool::CreateMemberJob<void>(game, &Game::Update);
	//ThreadPool::CreateMemberJob<void>(player->leftController, &ControllerObject::Update);
	//ThreadPool::CreateMemberJob<void>(player->rightController, &ControllerObject::Update);

	//ThreadPool::CreateMemberJob<void>(scrollMan, &ScrollingUVManager::Update);
	//ThreadPool::CreateMemberJob<void>(rendInter, &Renderer::Render);

	if(!game->IsPaused()) {
		phyMan->Update();
		audioMan->Update();

	}
	else {
		player->PausedUpdate();
		phyMan->PausedUpdate();
	}

	inputMan->HandleInput();
	game->Update();
	player->leftController->Update();
	player->rightController->Update();

	scrollMan->Update();
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
	delete scrollMan;
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
	while(game->IsRunning()) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Loop();
			wnd.UpdateTitle(std::to_wstring(GhostTime::FrameRate()));
		}
	}

	CleanUp();

	Console::Free();

	return (int)msg.wParam;
}
