
#include "Turret.h"

#include <DirectXMath.h>



Turret::Turret() {
	tag = std::string("Turret");
}

Turret::~Turret()
{
}

void Turret::OnCollision(GameObject* object)
{
	if(object->GetTag().c_str() == "Enemy")
	{
		using namespace DirectX;

		DirectX::XMVECTOR pos = DirectX::XMLoadFloat(position.m[0]);
		DirectX::XMVECTOR enemypos = DirectX::XMLoadFloat(object->position.m[0]);
		pos = DirectX::XMVector3Length(pos - enemypos);
		XMVectorAbs(pos);
		float length;
		XMStoreFloat(&length, pos);
		
		if( length < targetDistance)
		{
			target = object;
		}
	}
}

