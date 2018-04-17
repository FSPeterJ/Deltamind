#include "Core.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "GhostTime.h"
#include "Wwise_IDs.h"
#define NORMALCOLOR {0.5f, 0.5f, 0.5f}
#define PANICCOLOR {1.0f, 0.0f, 0.0f}

void Core::Awake(Object* obj) {
	GameObject::Awake(obj);
	/*
	for (int i = 0; i < cubeCount; ++i) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[i]));
	}
	*/
	panicTimer = -1;
	panicDuration = 4;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
	light.SetAsPoint(NORMALCOLOR, transform.GetPosition(), 1000);
	light.Enable();
	SetToFullHealth();
	panicking = false;
}
void Core::Update() {
	float dt = (float)GhostTime::DeltaTime();
	
	//-----TODO: TEMP. Should be done in awake, (can't now because position is set after awake is called)
	/*
	float radius = 0.5f;
	float height = 2;

	for (int i = 0; i < cubeCount; ++i) {
		float xVal = sinf(DirectX::XMConvertToRadians((360.0f / cubeCount)*i)) * radius;
		float zVal = cosf(DirectX::XMConvertToRadians((360.0f / cubeCount)*i)) * radius;
		healthCubes[i]->transform.SetPosition({ transform.GetPosition().x + xVal, transform.GetPosition().y + height, transform.GetPosition().z + zVal });
	}
	*/
	light.transform.SetPosition({ transform.GetPosition().x, transform.GetPosition().y + 2, transform.GetPosition().z });
	//---- END TODO

	if (panicTimer != -1) {
		if (panicTimer >= panicDuration) {
			panicTimer = -1;
			light.SetColor(NORMALCOLOR);
			panicking = false;
			MessageEvents::SendMessage(EVENT_CoreStopDamaged, EventMessageBase());
		}
		else {
			panicTimer += dt;
		}
	}
}
void Core::HealedEvent() {
}
void Core::HurtEvent() {
	panicTimer = 0;
	if (!panicking)
		MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_COREALARM));
	light.SetColor(PANICCOLOR);
	panicking = true;
	Core const* core = this;
	MessageEvents::SendMessage(EVENT_CoreDamaged, CoreMessage(&core));
	/*
	for (int i = 0; i < cubeCount; ++i) {
		if (PercentHealth() <= i * (1.0f/(float)cubeCount)) {
			if (healthCubes[i]) {
				MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
					if (this->healthCubes[i])
						healthCubes[i]->Destroy();
				});
			}
		}
	}
	*/
}
void Core::DeathEvent() {
	/*
	MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
		if (healthCubes[0]) 
			healthCubes[0]->Destroy();
	});
	*/
	panicTimer = -1;
	light.SetColor(NORMALCOLOR);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	light.SetColor({ 0, 0, 0 });
	light.RemoveLightFromManager();
	MessageEvents::SendMessage(EVENT_UnregisterNoisemaker, NewObjectMessage(this));
	GameObject::Destroy();
}