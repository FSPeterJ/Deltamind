#pragma once
#include "EngineStructure.h"
#include "Grid.h"
#include "HexagonalGridLayout.h"
#include "HexGrid.h"

class Game {
	HexagonalGridLayout flat = HexagonalGridLayout(layout_flat, DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT2(0, 0));
	HexGrid hexGrid = HexGrid(4);
public:
	Game() {
		EngineStructure::Update += [=]() {
			this->Update();
		};
	}

	Object * test, *test2;

	//Grid mygrid = Grid(10, 10, 1, 1, 0, 0);

	void ThreadTest(int test) {
		Console::Write << test;
	}

	void Start() {
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,2,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { -2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(6, DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f)));
		//mygrid.GreatGrid();
		hexGrid.Fill();
	}

	
	void Update() {
		//mygrid.Draw();
		hexGrid.Display(flat);
	}
	void Clean() {}
};
