#pragma once
#include "Object.h"
#include "Delegate.h"

class EngineSystem {

	Delegate<> Awake;

	void i() {
		//Awake( );
	}


};



class GameObject: public Object {


	virtual void Awake() {

	}

};
