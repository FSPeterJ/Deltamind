#include "GhostTime.h"
#include "EngineStructure.h"

XTime GhostTime::timer;

void GhostTime::Initalize() {
	timer.Restart();
	EngineStructure::Update += [=]() { Tick(); };
}
