#pragma once
#include <chrono>
#include "XTime.h"
#undef SendMessage

class GhostTime {
	static XTime timer;
	static void Tick();
	friend void Loop();
	static double timeScale;
public:
	struct Moment {
		std::chrono::system_clock::time_point moment;
		Moment(std::chrono::system_clock::time_point&& m) noexcept : moment(std::move(m)) {}
		Moment() {}
	};

	static Moment Now();
	static __int64 Duration(Moment, Moment);
	static void Initalize();

	static double DeltaTime() { return timer.Delta() * timeScale; }
	static double SmoothDeltaTime() { return timer.SmoothDelta() * timeScale; }
	static double FrameRate() { return timer.SamplesPerSecond(); }
	static void ToggleTimeScale();
	static double DeltaTimeDebug() { return 0.5; };

};
