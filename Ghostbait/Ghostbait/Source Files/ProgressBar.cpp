#include "ProgressBar.h"

void ProgressBar::SetBarPercentage(float percent) {
	percentage = percent;
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._11 *= percentage;
	newPos._12 *= percentage;
	newPos._13 *= percentage;
	SetPosition(newPos);
}