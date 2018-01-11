#pragma once
#include <functional>

class Game {

public:
	Game( ) { }

	Object * test, *test2;

	void ThreadTest(int test) {
		Write(test);
		WriteLine("     Fuck yes");
	}

	void Start() {
		Object * test = ObjectFactory::CreatePrefab(&std::string("BaseClass"));
		Object * test2 = ObjectFactory::CreatePrefab(&std::string("TestObject"));

		MessageEvents::SendMessage(EVENT_Instantiate, InstantiateMessage(0, {0,0,0,0}));

		XMFLOAT4 test1newpos = XMFLOAT4(2.0f, 1.0f, 0.0f, 1.0f);
		test->position.r[3] = XMLoadFloat4(&test1newpos);
		//test->testing();
		//((TestObject*)test2)->testing();

		//rendInter->registerObject(test);
		//rendInter->registerObject(test2);
	}

	void Update() {


	}

	void Clean() {
		delete test;
		delete test2; 
	}

};
