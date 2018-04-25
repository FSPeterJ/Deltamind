#include "GhostTime.h"
//#include "EngineStructure.h"
//#include <ctime>

XTime GhostTime::timer;
double GhostTime::timeScale = 1;

void GhostTime::Initalize() {
	timer.Restart();
	timeScale = 1;
	//EngineStructure::Update += [=]() { Tick(); };
}

void GhostTime::Tick() { 
	timer.Signal(); 
}

GhostTime::Moment GhostTime::Now() {
	return Moment(std::chrono::system_clock::now());
}

__int64 GhostTime::Duration(Moment start, Moment end) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(end.moment - start.moment).count();
}

void GhostTime::ToggleTimeScale() {
	if (timeScale == 1)
		timeScale = 0.25;
	else timeScale = 1;
}