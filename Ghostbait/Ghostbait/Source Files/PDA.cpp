#include "PDA.h"

PDA::PDA() {
	SetTag("PDA");
}

void PDA::Awake(Object* obj) {
	Item::Awake(obj);
}
void PDA::ActiveUpdate() {
	Item::ActiveUpdate();
	transform.NormalizeAllAxis();
	float dist = (hand == HAND_Left ? distance : -distance);
	DirectX::XMFLOAT4X4 mat = transform.GetMatrix();
	mat._41 += (mat._11 * dist);
	mat._42 += (mat._12 * dist);
	mat._43 += (mat._13 * dist);
	transform.SetMatrix(mat);
}