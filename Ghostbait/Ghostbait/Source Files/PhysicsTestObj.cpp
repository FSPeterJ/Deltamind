#include "PhysicsTestObj.h"
#include "GhostTime.h"
#include "Console.h"

PhysicsTestObj::PhysicsTestObj() {
	SetTag("PhysicsTestObj");
}


PhysicsTestObj::~PhysicsTestObj() {
}

void PhysicsTestObj::Update() {
	float dt = (float)GhostTime::DeltaTime();
	if (!isControllable) return;

	if (KeyIsDown(Control::TestInputU))
	{
		position._42 += 10.0f * dt;
		ResetKey(Control::TestInputU);
	}
	if (KeyIsDown(Control::TestInputO))
	{
		position._42 -= 10.0f * dt;
		ResetKey(Control::TestInputO);
	}
	if (KeyIsDown(Control::TestInputI))
	{
		position._43 += 10.0f * dt;
		ResetKey(Control::TestInputI);
	}
	if (KeyIsDown(Control::TestInputK))
	{
		position._43 -= 10.0f * dt;
		ResetKey(Control::TestInputK);
	}
	if (KeyIsDown(Control::TestInputJ))
	{
		position._41 -= 10.0f * dt;
		ResetKey(Control::TestInputJ);
	}
	if (KeyIsDown(Control::TestInputL))
	{
		position._41 += 10.0f * dt;
		ResetKey(Control::TestInputL);
	}
	
}

void PhysicsTestObj::OnCollision(GameObject* _other)
{
	//Console::WriteLine << "Colliding";
}