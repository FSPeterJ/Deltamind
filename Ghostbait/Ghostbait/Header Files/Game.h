#pragma once

class Game {

public:
	Game() { }

	Object * test, *test2;

	void ThreadTest(int test) {
		Write(test);
	}

	void Start() {

		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,2,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 2,0,0,1 }));
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { -2,0,0,1 }));

	}

	void Update() {


	}

	void Clean() {

	}

};
