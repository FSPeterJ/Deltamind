#include "ProgressBar.h"
#include "DebugRenderer.h"
void ProgressBar::Awake(Object* obj) {
	GameObject::Awake(obj);
	SetBarPercentage(0);
}
void ProgressBar::SetBarPercentage(float percent) {
	percentage = percent;
	DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
	newPos._11 *= percentage;
	newPos._12 *= percentage;
	newPos._13 *= percentage;
	transform.SetMatrix(newPos);
}