#include <windows.h>
#include "Resource.h"

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#define BREAK_AT -1
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "Window.h"

#include "Console.h"
#include "Renderer.h"
using namespace Console;

#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "TestObject.h"


#include "VRManager.h"

#include "InputManager.h"
#include "Messagebox.h"

Renderer* rendInter;
VRManager* vrMan;

void Setup(HINSTANCE hInstance, int nCmdShow) {

	Window wnd(900, 900);

	if (!wnd.Initialize(hInstance, nCmdShow)) { Messagebox::ShowError(L"Error!!", L"Main window is not initialized!"); }
	wnd.UpdateTitle(L"Ghostbait");

	Allocate();
	WriteLine("App has been initalized!");
	//Minimize();

	vrMan = new VRManager();
	vrMan->Init();
	rendInter = new Renderer();
	rendInter->Initialize(wnd, vrMan);

	//Object Factory Testing
	//====================================
	ObjectFactory::Initialize(rendInter->getMeshManager());
	ObjectFactory::Register<Object>(Object().GetTypeId());
	ObjectFactory::Register<TestObject>(TestObject().GetTypeId());

	MessageEvents::SendMessage(EVENT_Instantiate, InstantiateMessage(0, {0,0,0,0} ));
}

void Loop() {
	InputManager::HandleInput();
	rendInter->Render();

}

void CleanUp() {
	if (vrMan) {
		delete vrMan;
	}
	if (rendInter) {
		rendInter->Destroy();
		delete rendInter;
	}
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

	Object * test = ObjectFactory::CreatePrefab(&std::string("BaseClass"));
	Object * test2 = ObjectFactory::CreatePrefab(&std::string("TestObject"));

	XMFLOAT4 test1newpos = XMFLOAT4(2.0f, 1.0f, 0.0f, 1.0f);
	test->position.r[3] = XMLoadFloat4(&test1newpos);
	//test->testing();
	//((TestObject*)test2)->testing();

	//====================================

	rendInter->registerObject(test);
	rendInter->registerObject(test2);
	MSG msg;
	while(true) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			Loop();
		}
	}

	//Test Objects
	delete test;
	delete test2;

	CleanUp();

	Free();

	return (int) msg.wParam;
}
