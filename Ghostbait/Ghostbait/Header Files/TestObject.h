#pragma once
#include "GameObject.h"

struct TestObject: public Object {
	int x = 133;
public:
	TestObject();
	~TestObject();
	CloneFunction(TestObject)

};

static int leakCtor = 0;
static int leakDtor = 0;
static int leakClean = 0;
class Leak: public Object {
	int* l = nullptr;
public:
	Leak() {
		leakCtor++;
		Debug("CREATE Leak()" << leakCtor);

		l = new int[1];
	}

	virtual ~Leak() {
		leakDtor++;
		Debug("DELETE ~Leak()" << leakDtor);
		if(l) {
			delete[] l;
			l = 0;
		}
	}
	virtual void CleanObject() {
		//leakClean++;
		//Debug("Leak clean()" << leakClean);
		//if(l) {
//		/	delete[] l;
		//	l = 0;
		//}
	}

};

static int anotherCtor = 0;
static int anotherDtor = 0;
static int anotherClean = 0;
struct AnotherTestObject: public Leak {
	int x = 95;
public:
	AnotherTestObject() {

		anotherCtor++;
		Debug("CREATE AnotherTestObject() " << anotherCtor << "   " << __LINE__ << "  " << __FILE__);

	}


	virtual ~AnotherTestObject() {
		anotherDtor++;
		Debug("DELETE ~AnotherTestObject()" << anotherDtor);
	}


	CloneFunction(AnotherTestObject)

};
