#include "PhysicsTestObj.h"
#include "GhostTime.h"
#include "Console.h"
#include "PhysicsExtension.h"

PhysicsTestObj::PhysicsTestObj() {
	SetTag("PhysicsTestObj");
}


PhysicsTestObj::~PhysicsTestObj() {
	int x = 00;
}

void PhysicsTestObj::Update() {
	float dt = (float)GhostTime::DeltaTime();
	if (!isControllable) return;
	if (isRayCasting)
		Raycast(&transform, DirectX::XMFLOAT3(transform.GetMatrix()._31, transform.GetMatrix()._32, transform.GetMatrix()._33));

	if (KeyIsDown(Control::TestInputU))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._42 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputU);
	}
	if (KeyIsDown(Control::TestInputO))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._42 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputO);
	}
	if (KeyIsDown(Control::TestInputI))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._43 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputI);
	}
	if (KeyIsDown(Control::TestInputK))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._43 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputK);
	}
	if (KeyIsDown(Control::TestInputJ))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._41 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputJ);
	}
	if (KeyIsDown(Control::TestInputL))
	{
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._41 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputL);
	}
	
}

void PhysicsTestObj::OnCollision(GameObject* _other)
{
	//Console::WriteLine << "Colliding";
}