#pragma once
#include "Delegate.h"
#include "Console.h"
using namespace Console;

class EngineStructure {
	friend class GameObjectComponent;
	friend class GameObject;
	friend class Controlable;

	static Delegate<> Awake;
	static Delegate<> OnEnable;
	static Delegate<> Start;

	static Delegate<> FixedUpdate;
	static Delegate<> OnTriggerEnter;
	static Delegate<> OnTriggerStay;
	static Delegate<> OnTriggerExit;
	static Delegate<> OnCollisionEnter;
	static Delegate<> OnCollisionStay;
	static Delegate<> OnCollisionExit;

	static Delegate<> Update;

	static Delegate<> LateUpdate;

	static Delegate<> OnWillRenderObject;
	static Delegate<> OnPreCull;
	static Delegate<> OnBecameVisible;
	static Delegate<> OnBecameInvisible;
	static Delegate<> OnPreRender;
	static Delegate<> OnRenderObject;
	static Delegate<> OnPostRender;
	static Delegate<> OnRenderImage;

	static Delegate<> OnPause;

	static Delegate<> OnQuit;
	static Delegate<> OnDisable;
	static Delegate<> OnDestroy;

public: 

	void ExecuteAwake() {
		Write("Awake delegate contains ");
		Write((int)Awake.subscriber_count());
		WriteLine(" subscribers.");

		Awake();
	}

	void ExecuteUpdate() {
		//Write("Update delegate contains ");
		//Write((int) Update.subscriber_count());
		//WriteLine(" subscribers.");

		Update();
	}

	 
};
