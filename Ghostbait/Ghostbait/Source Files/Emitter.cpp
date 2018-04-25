#include "Emitter.h"
#include "ParticleTextureManager.h"
#include "MessageEvents.h"
#include "EngineStructure.h"
#include "GhostTime.h"
#include "Object.h"
#include "MessageStructs.h"

void Emitter::AddMaterial(Material * mat) {
	//Materials[0] = (ComponentBase*) mat;

}




void Emitter::Update() {
	float dt = (float)GhostTime::DeltaTime();
	age += dt;
	if(lifespan && age > lifespan) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });

	}
	if(parentObject) {
		transform.matrix = parentObject->transform.matrix;
	}
	mainData.Position = transform.GetPosition();
	mainData.emissionOverflow = previousOverflow;
	if(mainData.emissionIntervalSec >= mainData.emissionOverflow + dt) {
		previousOverflow += dt;
	}
	else {
		previousOverflow = fmodf((mainData.emissionOverflow + dt), mainData.emissionIntervalSec);
	}

}


void Emitter::Enable() {
	enabled = true;
	if(!updateID) {
		updateID = EngineStructure::Update.Add([=]() { Update(); });
	}
}
// Will disable the object after Update main loop is complete
void Emitter::Disable() {
	enabled = false;
	assert(updateID != 0);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {
		if(updateID != 0) {
			EngineStructure::Update.Remove(updateID);
			updateID = 0;
		}
	});
}

void Emitter::Destroy() {
	Disable();
	MessageEvents::SendMessage(EVENT_DeleteEmitter, DeleteEmitterMessage(this));
}