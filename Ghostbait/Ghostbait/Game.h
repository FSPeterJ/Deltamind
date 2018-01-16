#pragma once
#include <functional>

class Game {

public:
	Game() { }

	Object * test, *test2;

	void ThreadTest(int test) {
		Write(test);
	}

	void Start() {
		ObjectFactory::CreatePrefab(&std::string("BaseClass"));
		ObjectFactory::CreatePrefab(&std::string("TestObject"));

		ObjectManager::Initialize();
		Object * temp;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,2,0,1 }));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { -2,0,0,1 }));

		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 2,0,0,1 }));


		temp->GetTypeId();
	}

	void Update() {


	}

	void Clean() {
		delete test;
		delete test2;
	}

};
