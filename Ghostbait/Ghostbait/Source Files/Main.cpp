#include <windows.h>
#include "Resource.h"

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
//#define BREAK_AT -1
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "Window.h"

#include "Console.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "MemoryManager.h"
using namespace Console;

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

Renderer* rendInter;
VRManager* vrMan;
Game* game;
InputManager* inputMan;
PhysicsManager* phyMan;
MemoryManager memorysys;


void ExecuteAsync() {
	WriteLine("I am executed asyncly!");
	throw std::invalid_argument("ERROR: This is a test showing we can know if a thread throws an exception on it's work.\n");
}

//void CleanUp();

void Setup(HINSTANCE hInstance, int nCmdShow) {


	Window wnd(900, 900);

	if(!wnd.Initialize(hInstance, nCmdShow)) { Messagebox::ShowError("Error!!", "Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	Allocate();
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


	//char * my_array = (char *)malloc(536870912);
	//if(my_array == nullptr) throw std::exception("Memory Error");

	//for(int i = 0; i < 2; ++i) {
	//	new (my_array + i * sizeof(Object)) Object();
	//}
	//Object * testing = (Object*) my_array;

	//my_array = memorysys.ReturnBuffer();
	_Pool_Base::RegisterMemory(&memorysys);

	Pool<Object> yaypool(128);

	//Object* address = (Object*)memorysys.RequestMemory(1, sizeof(Object));
	//for(int i = 0; i < 2; ++i) {
	//	new (my_array + i * sizeof(Object)) Object();
	//}


	//Object* supertemp = new (address) Object();



	exit(0);
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


	if(vrMan->Init()) {
		rendInter->Initialize(wnd, vrMan);
		inputMan = new InputManager(VR, vrMan);
	}
	else {
		WriteLine("VR not initialized! Defaulting to 2D");
		rendInter->Initialize(wnd, nullptr);
		inputMan = new InputManager(KEYBOARD);
	}
	//Object* test = new Object();
	//test->SetComponent<Mesh>(rendInter->getMeshManager()->GetElement(UINT_MAX));
	//Object* test2 = new Object();
	////	int rr = test2->SetComponent<SomeCoolComponent>(r);

	//int dd = test2->SetComponent<Mesh>(rendInter->getMeshManager()->GetElement(UINT_MAX));
	//Mesh* temp1 = test->GetComponent<Mesh>();
	//Mesh* temp2 = test2->GetComponent<Mesh>();
	phyMan = new PhysicsManager();

	ObjectFactory::Initialize();
	ObjectFactory::RegisterPrefabBase<Object>(0);
	ObjectFactory::RegisterManager<Mesh, MeshManager>(rendInter->getMeshManager());
	ObjectFactory::RegisterManager<PhysicsComponent, PhysicsManager>(phyMan);

	TypeMap::RegisterComponent<Mesh>("Mesh");
	TypeMap::RegisterComponent<PhysicsComponent>("Physical");

	game = new Game();
	game->Start();
	vrMan->CreateControllers();


	Object* cube1, *cube2;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0,-1,0,1 }, &cube1));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0,-3,0,1 }, &cube2));
	cube1->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(0.5f, -1.0f, 0.0f);

}

void Loop() {
	rendInter->Render();
	game->Update();
	inputMan->HandleInput();
	phyMan->Update(0.0002f);
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
	ObjectManager::Shutdown();
	if(game) { game->Clean(); delete game; }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);

#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#ifdef BREAK_AT
	_CrtSetBreakAlloc(BREAK_AT);
#endif
#endif

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
