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
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
	light.SetAsPoint(NORMALCOLOR, transform.GetPosition(), 1000);
	light.Enable();
	SetToFullHealth();
	panicking = false;
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
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	light.SetColor({ 0, 0, 0 });
	light.RemoveLightFromManager();
	if (coreRing) coreRing->Destroy();
	MessageEvents::SendMessage(EVENT_UnregisterNoisemaker, NewObjectMessage(this));
	Core const* co = this;
	MessageEvents::SendMessage(EVENT_CoreDestroyed, CoreMessage(&co));
	GameObject::Destroy();
}
