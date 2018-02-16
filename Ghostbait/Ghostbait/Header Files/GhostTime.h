#pragma once
#include "XTime.h"
#undef SendMessage

class GhostTime {
	static XTime timer;

	static void Tick() { timer.Signal(); }
	friend void Loop();
public:

	static void Initalize();

	static double DeltaTime() { return timer.Delta(); }
	static double SmoothDeltaTime() { return timer.SmoothDelta(); }
};
