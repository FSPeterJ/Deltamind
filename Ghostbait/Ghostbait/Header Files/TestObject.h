#pragma once
#include "GameObject.h"

struct TestObject: public Object {
	int x = 133;
public:
	TestObject();
	~TestObject();
	CloneFunction(TestObject)

};

class Leak : public Object {
	int* l = nullptr;
public:
	Leak() {
		l = new int[1];
	}

	virtual ~Leak() {
		if(l) {
			delete[] l;
			l = 0;
		}
	}
	//virtual void clean() {
	//	if(l) {
	//		delete[] l;
	//		l = 0;
	//	}
	//}

};

struct AnotherTestObject: public Leak {
	int x = 95;
public:
	AnotherTestObject(){
	
		Debug("Creating a AnotherTestObject : Leak... " << __LINE__ << "  " << __FILE__);
	
	
	}
	//virtual void clean() {
	//	Leak::clean();
	//}
	virtual ~AnotherTestObject(){}


	CloneFunction(AnotherTestObject)

};
