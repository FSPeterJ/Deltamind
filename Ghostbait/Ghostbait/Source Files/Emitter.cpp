#include "Emitter.h"
#include "ParticleTextureManager.h"
#include "MessageEvents.h"
#include "EngineStructure.h"
#include "GhostTime.h"

void Emitter::AddMaterial(Material * mat) {
	//Materials[0] = (ComponentBase*) mat;

}




void Emitter::Update() {
	float dt = (float)GhostTime::DeltaTime();
	mainData.emissionOverflow = previousOverflow;
	if(mainData.emissionIntervalSec >= mainData.emissionOverflow + dt) {
		previousOverflow += dt;
	}
	else {
		previousOverflow = fmodf((mainData.emissionOverflow + dt), mainData.emissionIntervalSec);
	}
}


void Emitter::Enable() {
	if(!updateID) {
		updateID = EngineStructure::Update.Add([=]() { Update(); });
	}
}
// Will disable the object after Update main loop is complete
void Emitter::Disable() {

	assert(updateID != 0);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {
		if(updateID != 0) {
			EngineStructure::Update.Remove(updateID);
			updateID = 0;
		}
	});
}