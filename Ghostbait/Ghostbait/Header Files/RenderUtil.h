#pragma once
#include <cstdlib>



inline float FloatRandomRange(float a, float b) {
	return a + (float)rand() / (float)RAND_MAX * b - a;
}