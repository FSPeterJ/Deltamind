#include "Core.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "GhostTime.h"
#include "Wwise_IDs.h"
#include "TextManager.h"
#include "Material.h"
#define NORMALCOLOR {0.5f, 0.5f, 0.5f}
#define PANICCOLOR {1.0f, 0.0f, 0.0f}

void Core::Awake(Object* obj) {
	GameObject::Awake(obj);
	/*
	for (int i = 0; i < cubeCount; ++i) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[i]));
	}
	*/
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/CoreRing.ghost")), { 0, 0, 0 }, &coreRing));
	coreRingMat = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "\n      Core Health 100%      \n", foreground, background).mat;
	coreRing->SetComponent<Material>(coreRingMat);
	coreRing->ToggleFlag(UNLIT);
	coreRing->UnRender();
	coreRing->RenderTransparent();
	gridRadius = 2;
	panicTimer = -1;
	panicDuration = 4;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, ObjectMessage(this));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_COREHUM));
	light.SetAsPoint(NORMALCOLOR, transform.GetPosition(), 1000);
	light.Enable();
	spots[0].SetAsSpot(NORMALCOLOR, { 20.0f, 10.0f, 0.0f }, { 0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);
	spots[1].SetAsSpot(NORMALCOLOR, { -20.0f, 10.0f, 0.0f }, { -0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);
	spots[2].SetAsSpot(NORMALCOLOR, { 27.0f, 10.0f, -51.0f }, { 0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);
	spots[3].SetAsSpot(NORMALCOLOR, { -27.0f, 10.0f, 52.0f }, { -0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);
	spots[4].SetAsSpot(NORMALCOLOR, { -26.0f, 10.0f, -51.0f }, { -0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);
	spots[5].SetAsSpot(NORMALCOLOR, { 26.0f, 10.0f, 52.0f }, { 0.45f, -0.55f, 0.0f }, 0.9f, 0.8f);

	SetToFullHealth();
	//Test(PercentHealth());
}
void Test(float s) {
	int i = 0;
}
void Core::Update() {
	float dt = (float)GhostTime::DeltaTime();
	
	light.transform.SetPosition({ transform.GetPosition().x, transform.GetPosition().y + 2, transform.GetPosition().z });
	if(coreRing) coreRing->transform.SetMatrix(transform.GetMatrix());

	if (panicTimer != -1) {
		if (panicTimer >= panicDuration) {
			panicTimer = -1;
			light.SetColor(NORMALCOLOR);
			for (int i = 0; i < 6; ++i)
				spots[i].SetColor(NORMALCOLOR);
			MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::STOP_SFX_COREALARM));
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
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_COREALARM));
	light.SetColor(PANICCOLOR);
	for (int i = 0; i < 6; ++i)
		spots[i].SetColor(PANICCOLOR);
	std::string message = "\n      Core Health ";
	message.append(std::to_string((int)(PercentHealth() * 100)) + "%      \n");
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, coreRingMat, foreground, background);

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
	for (int i = 0; i < 6; ++i)
		spots[i].SetColor(NORMALCOLOR);
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::STOP_SFX_COREALARM));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::STOP_SFX_COREHUM));
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	light.SetColor({ 0, 0, 0 });
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::STOP_SFX_COREALARM));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::STOP_SFX_COREHUM));
	light.RemoveLightFromManager();
	for (int i = 0; i < 6; ++i)
		spots[i].RemoveLightFromManager();
	if (coreRing) coreRing->Destroy();
	MessageEvents::SendMessage(EVENT_UnregisterNoisemaker, ObjectMessage(this));
	Core const* co = this;
	MessageEvents::SendMessage(EVENT_CoreDestroyed, CoreMessage(&co));
	GameObject::Destroy();
}
