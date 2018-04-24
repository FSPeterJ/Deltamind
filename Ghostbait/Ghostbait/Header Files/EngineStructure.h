#pragma once
#include "Delegate.h"
#include "Console.h"
using namespace Common;

class EngineStructure {
	friend class GameObjectComponent;
	friend class GameObject;
	friend class Controlable;
	friend class ObjectManager;
	friend class Camera;
	friend class MessageEvents;
	friend class Game;
	friend class Animator;
	friend class GhostTime;
	friend class Emitter;
	friend class AntColony;

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
	static Delegate<> AnimationUpdate;
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
		Console::WriteLine << "Awake delegate contains " << Awake.subscriber_count() << " subscribers.";

		Awake();
	}


	void ExecuteAnimationUpdate() {
		//Write("Update delegate contains ");
		//Write((int) Update.subscriber_count());
		//WriteLine(" subscribers.");

		AnimationUpdate();
	}

	void ExecuteUpdate() {
		//Write("Update delegate contains ");
		//Write((int) Update.subscriber_count());
		//WriteLine(" subscribers.");

		Update();// .RunAsync();
	}

	void ExecuteAnimationUpdate() {
		AnimationUpdate();
	}

	void ExecuteLateUpdate() {
		//Write("Update delegate contains ");
		//Write((int) Update.subscriber_count());
		//WriteLine(" subscribers.");

		LateUpdate();
	}
};
