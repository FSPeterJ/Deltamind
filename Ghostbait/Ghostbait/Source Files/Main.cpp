#include <windows.h>
#include "vld.h" //Before commenting this out, please see TODO below and then don't comment this out...
#include "Resource.h"
#ifndef NDEBUG
//#define _CRTDBG_MAP_ALLOC
//#define BREAK_AT 610
//#include <stdlib.h>
//#include <crtdbg.h>
#endif

#include "Window.h"

#include "Console.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "MemoryManager.h"
#include "GameObject.h"
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

#include "XTime.h"

Renderer* rendInter;
VRManager* vrMan;
Game* game;
InputManager* inputMan;
PhysicsManager* phyMan;
MemoryManager MemMan;
ObjectManager* objMan;
XTime timer;

//
/*

		MMP""MM""YMM   .g8""8q. `7MM"""Yb.     .g8""8q.  
		P'   MM   `7 .dP'    `YM. MM    `Yb. .dP'    `YM.
			 MM      dM'      `MM MM     `Mb dM'      `MM
			 MM      MM        MM MM      MM MM        MM
			 MM      MM.      ,MP MM     ,MP MM.      ,MP
			 MM      `Mb.    ,dP' MM    ,dP' `Mb.    ,dP'
		   .JMML.      `"bmmd"' .JMMmmmdP'     `"bmmd"'  
	==============================================================
	==============================================================

	-EVERYONE do this:
		+ Download Visual Leak Detector from here:
			https://github.com/KindDragon/vld/releases

		+ Open and copy the dbghelp.dlls from the VS2017 folder into the VDL folder since it doesn't natively support VS2017:

			E:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\Extensions\CppUnitFramework\x64	<- has the 64x version of dbghelp.dll
			E:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\CommonExtensions\Microsoft\TestWindow\Extensions\CppUnitFramework\		<- has the 86x version of dbghelp.dll

			E:\Program Files (x86)\Visual Leak Detector\bin\Win64 <- COPY the 64x version of dbghelp.dll in here
			E:\Program Files (x86)\Visual Leak Detector\bin\Win32 <- COPY the 86x version of dbghelp.dll in here
		
		if you want to debug on a desktop or another computer, be sure to add Program Files (x86)\Visual Leak Detector\inc to the C++ includes
		vdl.h does not compile in release and is automaticallSy ignored.
		we can then get rid of the obnoxious CRT memory debug on breakpoint method which is annoying

	== Objects ==
	- Re-test to be sure that new object manager works as expected after merging back into main with Pool_Test && Memory_Management (Large Malloc)
		+ Test recycling of an object
		+ Verify that the memory block we get (roughly 500MB) is passing out memory address correcty (no stepping on toes or re-righting our neighbors)
	- Objects need to have a destroy delegate that 
	- Clean up Object Manager constructor.  Maybe Create the Pools using std::vector, then when Initialize is called, check the number of registerd classes and std::move() the pools into managed heap space in an array.
	- Seperate template instance counting of Objects from Components (See TypeMapping.h for functionality).  MAybe just have two differently named functions and incrementors
	- Replace factory manual dummy loading with actual loading of a ghostbait object file

	== Engine ==
	- Add delta time
	- fix ALL warnings
	- Create a standard header with COMMON includes and attach it to .cpp's that use those things.
	- find any range based loops that do not use by reference ie: for(auto & element : container) and change it to use reference
	- Re-evaluate usages of singletons / static classes vs instanced classes (does factory need to be pure static?  is this anti-architecture?) 

	== Physics ==	
	- Investigate if Physics call collisions twice IE: Object A hits Object B so there is an event (A hit B) but ALSO an event (B hit A) which is redundant processing
	- The prefabs with collider samples are given live colliders and generate collisions at origin.  This is weird.
	- Research how much we can get away with having XMMATRIX and XMVECTOR use instead of XMFLOAT4x4 and XMFLOAT4
		It's very not cool loading and storing or memcpying just to do one or two lines of math.

	== Memory && Input ==


End of TODO
=====================================================================================================================================
-------------------------------------------------------------------------------------------------------------------------------------
=====================================================================================================================================
*/


void ExecuteAsync() {
	WriteLine("I am executed asyncly!");
	throw std::invalid_argument("ERROR: This is a test showing we can know if a thread throws an exception on it's work.\n");
}

//void CleanUp();

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


	if(vrMan->Init()) {
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
	ObjectFactory::RegisterPrefabBase<Object>();
	ObjectFactory::RegisterPrefabBase<SomeLeakyObject>();
	ObjectFactory::RegisterManager<Mesh, MeshManager>(rendInter->getMeshManager());
	ObjectFactory::RegisterManager<PhysicsComponent, PhysicsManager>(phyMan);

	TypeMap::RegisterComponent<Mesh>("Mesh");
	TypeMap::RegisterComponent<PhysicsComponent>("Physical");



	ObjectFactory::CreatePrefab(&std::string("BaseClass"));
	ObjectFactory::CreatePrefab(&std::string("TestObject"));

	game = new Game();
	game->Start();
	vrMan->CreateControllers();


	Object* cube1, *cube2;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0,-1,0,1 }, &cube1));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0,-3,0,1 }, &cube2));
	cube1->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(0.5f, -1.0f, 0.0f);
	cube2->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(1.0f, 0.0f, 0.0f);

	//MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(cube1));


	//Initialize XTime
	timer.Restart();
}

void Loop() {
	timer.Signal();
	rendInter->Render();
	game->Update();
	inputMan->HandleInput();
	phyMan->Update((float)timer.SmoothDelta());
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
	if(game)
	{
		game->Clean(); 
		delete game;
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);

//#ifndef NDEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#ifdef BREAK_AT
//	_CrtSetBreakAlloc(BREAK_AT);
//#endif
//#endif

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
