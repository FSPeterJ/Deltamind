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
using namespace Console;

#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "TestObject.h"

#include "GameObject.h"
#include "EngineStructure.h"


#include "VRManager.h"

#include "InputManager.h"
#include "Messagebox.h"

#include "Game.h"
#include "ThreadPool.h"


Renderer* rendInter;
VRManager* vrMan;
Game* game;

void ExecuteAsync() {

	WriteLine("I am executed asyncly!");

}


void Setup(HINSTANCE hInstance, int nCmdShow) {

	Window wnd(900, 900);

	if(!wnd.Initialize(hInstance, nCmdShow)) { Messagebox::ShowError(L"Error!!", L"Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	Allocate();
	WriteLine("App has been initalized!");
	//Minimize();


	EngineStructure engine;


	SomeCoolObject* testObj = new SomeCoolObject();

	engine.ExecuteAwake();

	delete testObj;

	Messagebox::ShowError(L"Exit", "Stopping");







	ThreadPool::Start();
	ThreadPool::MakeJob(ExecuteAsync);


	vrMan = new VRManager();
	rendInter = new Renderer();
	if(vrMan->Init())
	{
		rendInter->Initialize(wnd, vrMan);
	}
	else
	{
		WriteLine("VR not initialized! Defaulting to 2D");
		rendInter->Initialize(wnd, nullptr);
	}




	ObjectManager::Initialize();

	//Object Factory Testing
	//====================================
	ObjectFactory::Initialize(rendInter->getMeshManager());
	ObjectFactory::Register<Object>(Object().GetTypeId());
	ObjectFactory::Register<TestObject>(TestObject().GetTypeId());

	game = new Game();
	game->Start();


}

void Loop() {
	InputManager::HandleInput();
	rendInter->Render();
	game->Update();
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

	//Object Factory Testing
	//====================================

	Object * test = ObjectFactory::CreateObject("BaseClass");
	Object * test2 = ObjectFactory::CreateObject("TestObject");
	Object * test3 = ObjectFactory::CreateObject("TestObject");
	Object * test4 = ObjectFactory::CreateObject("TestObject");


	XMFLOAT4 test1newpos = XMFLOAT4(0.0f, 0.5f, 2.0f, 1.0f);
	test->position.r[3] = XMLoadFloat4(&test1newpos);

	//test->testing();
	//((TestObject*)test2)->testing();

	//====================================

	rendInter->registerObject(test);
	rendInter->registerObject(test2);
	//rendInter->registerObject(test3);
	//rendInter->registerObject(test4);

	MSG msg;
	while(true) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			test2->position = vrMan->hmdPose;
			//test3->position = XMMatrixTranspose(XMLoadFloat4x4(&(rendInter->leftEye.camera.view)));
			//test4->position = XMMatrixTranspose(XMLoadFloat4x4(&(rendInter->rightEye.camera.view)));

			Loop();
		}
	}

	CleanUp();

	Free();

	return (int)msg.wParam;
}
