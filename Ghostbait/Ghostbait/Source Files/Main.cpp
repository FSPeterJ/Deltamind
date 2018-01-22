#include <windows.h>
#include "vld.h" //Before commenting this out, please see TODO below and then don't comment this out...
#include "Resource.h"
#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
//#define BREAK_AT 610
#include <stdlib.h>
#include <crtdbg.h>
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

Renderer* rendInter;
VRManager* vrMan;
Game* game;
InputManager* inputMan;
PhysicsManager* phyMan;
MemoryManager MemMan;
ObjectManager* objMan;

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
		+ Objects have their destructors called and do not leak any data
		+ Mutliple types of object children can be initialized
		+ Test recycling of an object
		+ Verify that the memory block we get (roughly 500MB) is passing out memory address correcty (no stepping on toes or re-righting our neighbors)
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
	- Solve this problem:

			 WARNING: Visual Leak Detector detected memory leaks!
			 ----------Block 236 at 0x00000000011843C0 : 16 bytes----------
			 Leak Hash : 0xE1477244, Count : 1, Total 16 bytes
			 Call Stack(TID 39024) :
			 ucrtbased.dll!malloc()
			 f : \dd\vctools\crt\vcstartup\src\heap\new_scalar.cpp(34): Ghostbait.exe!operator new() + 0xA bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(67): Ghostbait.exe!std::_Default_allocate_traits<1>::_Get_bytes() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(139): Ghostbait.exe!std::_Allocate<std::_Default_allocate_traits<1> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(1056): Ghostbait.exe!std::allocator<std::_Container_proxy>::allocate()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(574): Ghostbait.exe!std::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >::_Alloc_proxy() + 0xE bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(558): Ghostbait.exe!std::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(753): Ghostbait.exe!std::_List_buy<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >::_List_buy<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > ><std::allocator<std::pair<enum Control const, int> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(822): Ghostbait.exe!std::list<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >::list<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xhash(201): Ghostbait.exe!std::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enum Control, std::hash<enum Control>, std::equal_to<enum Control> >, std::allocator<std::pair<enum Control const, int> >, 0> >::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enu() + 0x1A bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\unordered_map(130): Ghostbait.exe!std::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum Control const, int> > >::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum()
	->		 Ghostbait.exe!InputManager::InputBridge::InputBridge() + 0x4E bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(109): Ghostbait.exe!InputManager::KeyboardInput::KeyboardInput() + 0x41 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(167): Ghostbait.exe!InputManager::SetInputType() + 0x27 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\header files\inputmanager.h(59): Ghostbait.exe!InputManager::InputManager()
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(139): Ghostbait.exe!Setup() + 0x40 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(259): Ghostbait.exe!wWinMain()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(123): Ghostbait.exe!invoke_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(283): Ghostbait.exe!__scrt_common_main_seh() + 0x5 bytes
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(326): Ghostbait.exe!__scrt_common_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_wwinmain.cpp(17): Ghostbait.exe!wWinMainCRTStartup()
			 KERNEL32.DLL!BaseThreadInitThunk() + 0x14 bytes
			 ntdll.dll!RtlUserThreadStart() + 0x21 bytes
			 Data :
			 F0 07 30 05    8A 01 00 00    00 00 00 00    00 00 00 00     ..0..... ........


			 ----------Block 237 at 0x0000000001184A00 : 16 bytes----------
			 Leak Hash : 0xA5209977, Count : 1, Total 16 bytes
			 Call Stack(TID 39024) :
			 ucrtbased.dll!malloc()
			 f : \dd\vctools\crt\vcstartup\src\heap\new_scalar.cpp(34): Ghostbait.exe!operator new() + 0xA bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(67): Ghostbait.exe!std::_Default_allocate_traits<1>::_Get_bytes() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(139): Ghostbait.exe!std::_Allocate<std::_Default_allocate_traits<1> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(1056): Ghostbait.exe!std::allocator<std::_Container_proxy>::allocate()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\vector(547): Ghostbait.exe!std::_Vector_alloc<std::_Vec_base_types<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > >, std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Contro() + 0xE bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\vector(500): Ghostbait.exe!std::_Vector_alloc<std::_Vec_base_types<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > >, std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Contro() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\vector(688): Ghostbait.exe!std::vector<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > >, std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > > > >::vec() + 0x13 bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xhash(202): Ghostbait.exe!std::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enum Control, std::hash<enum Control>, std::equal_to<enum Control> >, std::allocator<std::pair<enum Control const, int> >, 0> >::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enu() + 0x26 bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\unordered_map(130): Ghostbait.exe!std::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum Control const, int> > >::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum()
	->		 Ghostbait.exe!InputManager::InputBridge::InputBridge() + 0x4E bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(109): Ghostbait.exe!InputManager::KeyboardInput::KeyboardInput() + 0x41 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(167): Ghostbait.exe!InputManager::SetInputType() + 0x27 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\header files\inputmanager.h(59): Ghostbait.exe!InputManager::InputManager()
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(139): Ghostbait.exe!Setup() + 0x40 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(259): Ghostbait.exe!wWinMain()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(123): Ghostbait.exe!invoke_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(283): Ghostbait.exe!__scrt_common_main_seh() + 0x5 bytes
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(326): Ghostbait.exe!__scrt_common_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_wwinmain.cpp(17): Ghostbait.exe!wWinMainCRTStartup()
			 KERNEL32.DLL!BaseThreadInitThunk() + 0x14 bytes
			 ntdll.dll!RtlUserThreadStart() + 0x21 bytes
			 Data :
			 08 08 30 05    8A 01 00 00    00 00 00 00    00 00 00 00     ..0..... ........


			 ----------Block 234 at 0x00000000053007E0: 88 bytes----------
			 Leak Hash : 0x15136660, Count : 1, Total 88 bytes
			 Call Stack(TID 39024) :
			 ucrtbased.dll!malloc()
			 f : \dd\vctools\crt\vcstartup\src\heap\new_scalar.cpp(34): Ghostbait.exe!operator new() + 0xA bytes
	->		 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(167): Ghostbait.exe!InputManager::SetInputType() + 0xA bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\header files\inputmanager.h(59): Ghostbait.exe!InputManager::InputManager()
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(139): Ghostbait.exe!Setup() + 0x40 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(259): Ghostbait.exe!wWinMain()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(123): Ghostbait.exe!invoke_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(283): Ghostbait.exe!__scrt_common_main_seh() + 0x5 bytes
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(326): Ghostbait.exe!__scrt_common_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_wwinmain.cpp(17): Ghostbait.exe!wWinMainCRTStartup()
			 KERNEL32.DLL!BaseThreadInitThunk() + 0x14 bytes
			 ntdll.dll!RtlUserThreadStart() + 0x21 bytes
			 Data :
			 30 4C C7 3A    F6 7F 00 00    00 00 80 3F    CD CD CD CD     0L. : .... ... ? ....
			 C0 43 18 01    8A 01 00 00    70 E0 0B 5F    8A 01 00 00.C...... p.._....
			 00 00 00 00    00 00 00 00    00 4A 18 01    8A 01 00 00     .........J......
			 C0 67 09 5F    8A 01 00 00    40 68 09 5F    8A 01 00 00.g._.... @h._....
			 40 68 09 5F    8A 01 00 00    07 00 00 00    00 00 00 00     @h._.... ........
			 08 00 00 00    00 00 00 00                                   ........ ........


			 ----------Block 238 at 0x000000005F0967C0: 128 bytes----------
			 Leak Hash : 0x9992D04E, Count : 1, Total 128 bytes
			 Call Stack(TID 39024) :
			 ucrtbased.dll!malloc()
			 f : \dd\vctools\crt\vcstartup\src\heap\new_scalar.cpp(34): Ghostbait.exe!operator new() + 0xA bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(67): Ghostbait.exe!std::_Default_allocate_traits<1>::_Get_bytes() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(139): Ghostbait.exe!std::_Allocate<std::_Default_allocate_traits<1> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(1056): Ghostbait.exe!std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > > >::allocate()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\vector(1549): Ghostbait.exe!std::vector<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > >, std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > > > >::_Re() + 0x1B bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\vector(1573): Ghostbait.exe!std::vector<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > >, std::allocator<std::_List_unchecked_iterator<std::_List_val<std::_List_simple_types<std::pair<enum Control const, int> > > > > >::res()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xhash(1127): Ghostbait.exe!std::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enum Control, std::hash<enum Control>, std::equal_to<enum Control> >, std::allocator<std::pair<enum Control const, int> >, 0> >::_Init()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xhash(205): Ghostbait.exe!std::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enum Control, std::hash<enum Control>, std::equal_to<enum Control> >, std::allocator<std::pair<enum Control const, int> >, 0> >::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enu() + 0x11 bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\unordered_map(130): Ghostbait.exe!std::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum Control const, int> > >::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum()
	->		 Ghostbait.exe!InputManager::InputBridge::InputBridge() + 0x4E bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(109): Ghostbait.exe!InputManager::KeyboardInput::KeyboardInput() + 0x41 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(167): Ghostbait.exe!InputManager::SetInputType() + 0x27 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\header files\inputmanager.h(59): Ghostbait.exe!InputManager::InputManager()
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(139): Ghostbait.exe!Setup() + 0x40 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(259): Ghostbait.exe!wWinMain()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(123): Ghostbait.exe!invoke_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(283): Ghostbait.exe!__scrt_common_main_seh() + 0x5 bytes
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(326): Ghostbait.exe!__scrt_common_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_wwinmain.cpp(17): Ghostbait.exe!wWinMainCRTStartup()
			 KERNEL32.DLL!BaseThreadInitThunk() + 0x14 bytes
			 ntdll.dll!RtlUserThreadStart() + 0x21 bytes
			 Data :
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....


			 ----------Block 235 at 0x000000005F0BE070: 24 bytes----------
			 Leak Hash : 0xA70EA6A4, Count : 1, Total 24 bytes
			 Call Stack(TID 39024) :
			 ucrtbased.dll!malloc()
			 f : \dd\vctools\crt\vcstartup\src\heap\new_scalar.cpp(34): Ghostbait.exe!operator new() + 0xA bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(67): Ghostbait.exe!std::_Default_allocate_traits<1>::_Get_bytes() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(139): Ghostbait.exe!std::_Allocate<std::_Default_allocate_traits<1> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xmemory0(1056): Ghostbait.exe!std::allocator<std::_List_node<std::pair<enum Control const, int>, void * __ptr64> >::allocate()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(656): Ghostbait.exe!std::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >::_Buynode0() + 0x19 bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(647): Ghostbait.exe!std::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >::_Buyheadnode()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(556): Ghostbait.exe!std::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >::_List_alloc<std::_List_base_types<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > > >() + 0xC bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(753): Ghostbait.exe!std::_List_buy<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >::_List_buy<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > ><std::allocator<std::pair<enum Control const, int> >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\list(822): Ghostbait.exe!std::list<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >::list<std::pair<enum Control const, int>, std::allocator<std::pair<enum Control const, int> > >()
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\xhash(201): Ghostbait.exe!std::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enum Control, std::hash<enum Control>, std::equal_to<enum Control> >, std::allocator<std::pair<enum Control const, int> >, 0> >::_Hash<std::_Umap_traits<enum Control, int, std::_Uhash_compare<enu() + 0x1A bytes
			 e : \program files(x86)\microsoft visual studio\2017\enterprise\vc\tools\msvc\14.12.25827\include\unordered_map(130): Ghostbait.exe!std::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum Control const, int> > >::unordered_map<enum Control, int, std::hash<enum Control>, std::equal_to<enum Control>, std::allocator<std::pair<enum()
	->		 Ghostbait.exe!InputManager::InputBridge::InputBridge() + 0x4E bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(109): Ghostbait.exe!InputManager::KeyboardInput::KeyboardInput() + 0x41 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\inputmanager.cpp(167): Ghostbait.exe!InputManager::SetInputType() + 0x27 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\header files\inputmanager.h(59): Ghostbait.exe!InputManager::InputManager()
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(139): Ghostbait.exe!Setup() + 0x40 bytes
			 e : \projects\final project\smoking - sandwich\ghostbait\ghostbait\source files\main.cpp(259): Ghostbait.exe!wWinMain()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(123): Ghostbait.exe!invoke_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(283): Ghostbait.exe!__scrt_common_main_seh() + 0x5 bytes
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_common.inl(326): Ghostbait.exe!__scrt_common_main()
			 f : \dd\vctools\crt\vcstartup\src\startup\exe_wwinmain.cpp(17): Ghostbait.exe!wWinMainCRTStartup()
			 KERNEL32.DLL!BaseThreadInitThunk() + 0x14 bytes
			 ntdll.dll!RtlUserThreadStart() + 0x21 bytes
			 Data :
			 70 E0 0B 5F    8A 01 00 00    70 E0 0B 5F    8A 01 00 00     p.._....p.._....
			 CD CD CD CD    CD CD CD CD                                   ........ ........


			 Visual Leak Detector detected 5 memory leaks(532 bytes).
			 Largest number used : 553663559 bytes.
			 Total allocations : 574620524 bytes.
			 Visual Leak Detector is now exiting.



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

	//my_array = MemMan.ReturnBuffer();
	_Pool_Base::RegisterMemory(&MemMan);

	//Object* address = (Object*)MemMan.RequestMemory(1, sizeof(Object));
	//for(int i = 0; i < 2; ++i) {
	//	new (my_array + i * sizeof(Object)) Object();
	//}


	//Object* supertemp = new (address) Object();



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


	//Debug("sizeof(Object) = " << sizeof(Object));

	//MakePrototype(TestObject)
	//MakePrototype(AnotherTestObject)
	//	
	//TestObject* o = Object::CreateObject<TestObject>();
	//Debug("I just made a " << Object::GetObjectTypeName(o).c_str() << " : " << o);
	////
	//AnotherTestObject* oo = Object::CreateObject<AnotherTestObject>();
	//Debug("I just made a " << Object::GetObjectTypeName(oo).c_str() << " : " << oo);

	//AnotherTestObject* oo2 = Object::CreateObject<AnotherTestObject>();
	//Debug("I just made a " << Object::GetObjectTypeName(oo2).c_str() << " : " << oo2);
	//Object::CleanUp();

//	oo->~AnotherTestObject();
	//delete oo;


	//auto bucket = Object::objectPool.GetBucket<decltype(*o)>();
//	TestObject* items = bucket->GetItems();
	
	//delete[] items;
	//delete bucket;


	//size = sizeof(*((TestObject*)o));
	//Console::Write("Size of it is: ");
	//Console::WriteLine(size);

	//delete o;

//	system("pause");
//	CleanUp();

	phyMan = new PhysicsManager();
	objMan = new ObjectManager(&MemMan, 20);
	objMan->Initialize();

	ObjectFactory::Initialize(objMan);
	ObjectFactory::RegisterPrefabBase<Object>();
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
	int x = 113;
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
