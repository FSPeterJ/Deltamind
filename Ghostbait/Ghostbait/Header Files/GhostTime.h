#pragma once
#include "XTime.h"

class GhostTime {
	static XTime timer;

	static void Tick() { timer.Signal(); }
public:

	static void Initalize();

	static double DeltaTime() { return timer.Delta(); }
	//static double SmoothDeltaTime() { return timer.SmoothDelta(); }
};
