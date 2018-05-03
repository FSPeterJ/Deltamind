#include "GhostTime.h"
//#include "EngineStructure.h"
//#include <ctime>

XTime GhostTime::timer;
double GhostTime::timeScale = 1;
double GhostTime::sloMoSpeed = 0.25;
double GhostTime::timeScaleDuration;
double GhostTime::lerpDuration;
double GhostTime::currentTimeScaleTime = 0;
bool GhostTime::paused = false;
GhostTime::State GhostTime::state = Normal;

void GhostTime::Initalize() {
	timer.Restart();
	timeScale = 1;
	SetState(Normal);
	//EngineStructure::Update += [=]() { Tick(); };
}

void GhostTime::SetState(State _state) {
	state = _state;
	switch (state) {
		case Normal:
			timeScale = 1;
			break;
		case SloMo:
			timeScale = sloMoSpeed;
			currentTimeScaleTime = timeScaleDuration;
			break;
		case LerpIn:
		case LerpOut:
			currentTimeScaleTime = lerpDuration;
			break;
	}
}

void GhostTime::Tick() { 
	timer.Signal(); 
	if (paused) return;
	switch (state) {
		case Normal:
			break;
		case SloMo:
			{
				if (timeScaleDuration == -1) return;
				currentTimeScaleTime -= timer.Delta();
				if (currentTimeScaleTime <= 0) SetState(LerpOut);
			}
			break;
		case LerpIn:
			{
				currentTimeScaleTime -= timer.Delta();
				if (currentTimeScaleTime <= 0) SetState(SloMo);
				else timeScale = sloMoSpeed + (currentTimeScaleTime / lerpDuration) * (1 - sloMoSpeed);
			}
			break;
		case LerpOut:
			{
				currentTimeScaleTime -= timer.Delta();
				if (currentTimeScaleTime <= 0) SetState(Normal);
				else timeScale = 1 + (currentTimeScaleTime / lerpDuration) * (sloMoSpeed - 1);
			}
			break;
	}
}

GhostTime::Moment GhostTime::Now() {
	return Moment(std::chrono::system_clock::now());
}

__int64 GhostTime::Duration(Moment start, Moment end) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(end.moment - start.moment).count();
}

void GhostTime::ToggleSloMo(double _timeScaleDuration, double _lerpDuration) {
	timeScaleDuration = _timeScaleDuration;
	lerpDuration = _lerpDuration;
	if (state == Normal || state == LerpOut) SetState(LerpIn);
	else SetState(LerpOut);
}
void GhostTime::TurnOnSloMo(double _timeScaleDuration, double _lerpDuration) {
	timeScaleDuration = _timeScaleDuration;
	lerpDuration = _lerpDuration;
	if (state == LerpIn) return;
	else if (state == Normal || state == LerpOut) SetState(LerpIn);
	else SetState(SloMo);
}
void GhostTime::TurnOffSloMo(double _timeScaleDuration, double _lerpDuration) {
	timeScaleDuration = _timeScaleDuration;
	lerpDuration = _lerpDuration;
	if (state == LerpOut) return;
	else if (state == SloMo || state == LerpIn) SetState(LerpIn);
	else SetState(Normal);
}