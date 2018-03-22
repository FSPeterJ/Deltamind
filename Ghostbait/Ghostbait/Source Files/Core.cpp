#include "Core.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "DebugRenderer.h"
#undef SendMessage

void Core::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[0]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[1]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[2]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[3]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[4]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[5]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[6]));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/healthCube.ghost")), { 0, 0, 0 }, &healthCubes[7]));
	SetToFullHealth();
}
void Core::Update() {
	float distance = 0.5f;
	float height = 2;
	healthCubes[0]->transform.SetPosition({ transform.GetPosition().x,				transform.GetPosition().y + height,		transform.GetPosition().z + distance	});
	healthCubes[1]->transform.SetPosition({ transform.GetPosition().x + distance,	transform.GetPosition().y + height,		transform.GetPosition().z + distance	});
	healthCubes[2]->transform.SetPosition({ transform.GetPosition().x + distance,	transform.GetPosition().y + height,		transform.GetPosition().z				});
	healthCubes[3]->transform.SetPosition({ transform.GetPosition().x + distance,	transform.GetPosition().y + height,		transform.GetPosition().z - distance	});
	healthCubes[4]->transform.SetPosition({ transform.GetPosition().x,				transform.GetPosition().y + height,		transform.GetPosition().z - distance	});
	healthCubes[5]->transform.SetPosition({ transform.GetPosition().x - distance,	transform.GetPosition().y + height,		transform.GetPosition().z - distance	});
	healthCubes[6]->transform.SetPosition({ transform.GetPosition().x - distance,	transform.GetPosition().y + height,		transform.GetPosition().z				});
	healthCubes[7]->transform.SetPosition({ transform.GetPosition().x - distance,	transform.GetPosition().y + height,		transform.GetPosition().z + distance	});
}
void Core::HealedEvent() {
}
void Core::HurtEvent() {
	MessageEvents::SendMessage(EVENT_CoreDamaged, EventMessageBase());
	for (int i = 0; i < 8; ++i) {
		if (PercentHealth() <= i * 0.125f) {
			if (healthCubes[i]) {
				MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
					if (this->healthCubes[i])
						healthCubes[i]->Destroy();
				});
			}
		}
	}
	/*
	if (PercentHealth() <= 0.875f) {
		if (healthCubes[7]) {
			MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
				if (this->healthCubes[3])
					healthCubes[3]->Destroy();
			});
		}
		if (PercentHealth() <= 0.5f) {
			if (healthCubes[2]) {
				MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
					if (this->healthCubes[2])
						healthCubes[2]->Destroy();
				});
			}
			if (PercentHealth() <= 0.25f) {
				if (healthCubes[1]) {
					MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
						if (this->healthCubes[1])
							healthCubes[1]->Destroy();
					});
				}
			}
		}
	}
	*/
}
void Core::DeathEvent() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=]() {
		if (this->healthCubes[0]) 
			healthCubes[0]->Destroy();
	});

	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	GameObject::Destroy();
}