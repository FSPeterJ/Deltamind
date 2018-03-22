#pragma once

namespace GhostMind {
	namespace GhostRand {
		double RandomDoubleOpenMax(double inclusiveMin, double exclusiveMax);
		int RandomIntOpenMax(int inclusiveMin, int exclusiveMax);
		double RandomDoubleClosedMax(double inclusiveMin, double inclusiveMax);
		int RandomIntClosedMax(int inclusiveMin, int inclusiveMax);
	}
}
