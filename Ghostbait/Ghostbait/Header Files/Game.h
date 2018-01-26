#pragma once
#include "EngineStructure.h"
class Game {
public:
	Game() {
		EngineStructure::Update += [=]() { 
			this->Update(); 
		};
	}

	Object * test, *test2;

	void ThreadTest(int test) {
		Write(test);
	}

	void Start() {
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,2,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { -2,0,0,1 }));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(6, DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f)));

	}

	void Update() {
	}

	void Clean() {
	}
};
