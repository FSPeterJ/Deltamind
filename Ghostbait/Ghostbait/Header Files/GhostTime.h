#pragma once
#include <chrono>
#include "XTime.h"
#undef SendMessage

#define DEFAULT_DURATION 10
#define DEFAULT_LERPDURATION 2

class GhostTime {
	enum State {
		Normal,
		SloMo,
		LerpIn,
		LerpOut,
	};
	static XTime timer;
	static void Tick();
	friend void Loop();
	static double timeScale;
	static double sloMoSpeed;
	static double timeScaleDuration;
	static double lerpDuration;
	static double currentTimeScaleTime;
	static State state;

	static void SetState(State _state);

public:
	static bool paused;
	struct Moment {
		std::chrono::system_clock::time_point moment;
		Moment(std::chrono::system_clock::time_point&& m) noexcept : moment(std::move(m)) {}
		Moment() {}
	};

	static Moment Now();
	static __int64 Duration(Moment, Moment);
	static void Initalize();

	static double DeltaTime() { return timer.Delta() * timeScale; }
	static double DeltaTimeNoScale() { return timer.Delta(); }
	static double SmoothDeltaTime() { return timer.SmoothDelta() * timeScale; }
	static double FrameRate() { return timer.SamplesPerSecond(); }
	static void ToggleSloMo(double _timeScaleDuration = DEFAULT_DURATION, double _lerpDuration = DEFAULT_LERPDURATION);
	static void TurnOnSloMo(double _timeScaleDuration = DEFAULT_DURATION, double _lerpDuration = DEFAULT_LERPDURATION);
	static void TurnOffSloMo(double _timeScaleDuration = DEFAULT_DURATION, double _lerpDuration = DEFAULT_LERPDURATION);
#ifdef _DEBUG
	static double DeltaTimeDebug() { return 0.5; };
#endif // _DEBUG

};
