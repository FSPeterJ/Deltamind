#pragma once
#include "EngineStructure.h"
#include "Grid.h"

class Game {
public:
	Game() {
		EngineStructure::Update += [=]() {
			this->Update();
		};
	}

	Object * test, *test2;

	Grid mygrid = Grid(20, 20, 1, 1, 0, 0);

	void ThreadTest(int test) {
		Console::Write << test;
	}

	void Start() {
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,2,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { -2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(6, DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f)));
		//mygrid.GreatGrid();
	}

	
	void Update() {
		//mygrid.Draw();
	}
	void Clean() {}
};
