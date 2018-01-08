#include <windows.h>
#include "Window.h"
#include "MessageEvents.h"


#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
//#define BREAK_AT -1
#include <stdlib.h>
#include <crtdbg.h>
#endif



#include "Console.h"
using namespace Console;

void Loop()
{
	

};

void Death()
{
	WriteLine("DEAD!");


}

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
	MessageEvents::Subscribe(EVENT_Player_Death, Death);
	EventBase * temp =  new EventBase(EVENT_Player_Death);
	MessageEvents::SendMessage(EVENT_Player_Death, temp);
	//Setup();

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

	Free();





	return (int) msg.wParam;
}
