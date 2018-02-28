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
		Raycast(DirectX::XMFLOAT3(GetPosition()._41, GetPosition()._42, GetPosition()._43), DirectX::XMFLOAT3(GetPosition()._31, GetPosition()._32, GetPosition()._33));

	if (KeyIsDown(Control::TestInputU))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._42 += dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputU);
	}
	if (KeyIsDown(Control::TestInputO))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._42 -= dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputO);
	}
	if (KeyIsDown(Control::TestInputI))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._43 += dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputI);
	}
	if (KeyIsDown(Control::TestInputK))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._43 -= dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputK);
	}
	if (KeyIsDown(Control::TestInputJ))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._41 -= dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputJ);
	}
	if (KeyIsDown(Control::TestInputL))
	{
		DirectX::XMFLOAT4X4 newPos = GetPosition();
		newPos._41 += dt;
		SetPosition(newPos);
		//ResetKey(Control::TestInputL);
	}
	
}

void PhysicsTestObj::OnCollision(GameObject* _other)
{
	//Console::WriteLine << "Colliding";
}