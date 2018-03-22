#pragma once
#include "GameObject.h"

class ProgressBar : public GameObject {
	float percentage = 1;


	void Update() {};
public:
	void SetBarPercentage(float percent);
};