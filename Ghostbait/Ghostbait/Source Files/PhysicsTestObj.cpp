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
		Raycast(DirectX::XMFLOAT3(position._41, position._42, position._43), DirectX::XMFLOAT3(position._31, position._32, position._33));

	if (KeyIsDown(Control::TestInputU))
	{
		position._42 += dt;
		//ResetKey(Control::TestInputU);
	}
	if (KeyIsDown(Control::TestInputO))
	{
		position._42 -= dt;
		//ResetKey(Control::TestInputO);
	}
	if (KeyIsDown(Control::TestInputI))
	{
		position._43 += dt;
		//ResetKey(Control::TestInputI);
	}
	if (KeyIsDown(Control::TestInputK))
	{
		position._43 -= dt;
		//ResetKey(Control::TestInputK);
	}
	if (KeyIsDown(Control::TestInputJ))
	{
		position._41 -= dt;
		//ResetKey(Control::TestInputJ);
	}
	if (KeyIsDown(Control::TestInputL))
	{
		position._41 += dt;
		//ResetKey(Control::TestInputL);
	}
	
}

void PhysicsTestObj::OnCollision(GameObject* _other)
{
	//Console::WriteLine << "Colliding";
}