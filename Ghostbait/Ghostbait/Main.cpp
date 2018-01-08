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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);

#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#ifdef BREAK_AT
	_CrtSetBreakAlloc(BREAK_AT);
#endif
#endif
	

	Window wnd(900, 900);

	if(!wnd.Initialize(hInstance, nCmdShow)) { return FALSE; }
	wnd.UpdateTitle(L"Ghostbait");
	

	Allocate();
	WriteLine("App has been initalized!");
	//Minimize();

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GHOSTBAIT));

	//Setup();
	Renderer * rendInter = new Renderer();
	rendInter->Initialize(wnd);
	MSG msg;
	while(true) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//Loop();
		}
	}
	rendInter->Destroy();
	delete rendInter;
	Free();


	return (int) msg.wParam;
}
