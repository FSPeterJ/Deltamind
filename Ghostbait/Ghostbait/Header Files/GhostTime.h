#pragma once
#include <chrono>
#include "XTime.h"
#undef SendMessage

class GhostTime {
	static XTime timer;
	static void Tick();
	friend void Loop();
public:
	struct Moment {
		std::chrono::system_clock::time_point moment;
		Moment(std::chrono::system_clock::time_point&& m) noexcept : moment(std::move(m)) {}
		Moment() {}
	};

	static Moment Now();
	static __int64 Duration(Moment, Moment);
	static void Initalize();

	static double DeltaTime() { return timer.Delta(); }
	static double SmoothDeltaTime() { return timer.SmoothDelta(); }
	static double FrameRate() { return timer.SamplesPerSecond(); }
};
