#include "ProgressBar.h"

void ProgressBar::SetBarPercentage(float percent) {
	percentage = percent;
	position._11 *= percentage;
	position._12 *= percentage;
	position._13 *= percentage;
}