#include "PhysicsTestObj.h"
#include "GhostTime.h"
#include "Console.h"
#include "PhysicsExtension.h"
#include "Emitter.h"

#include "ParticleManager.h"
#include "MaterialManager.h"

PhysicsTestObj::PhysicsTestObj() {
	SetTag("PhysicsTestObj");
}


PhysicsTestObj::~PhysicsTestObj() {
	int x = 00;
}

void PhysicsTestObj::Update() {
	float dt = (float)GhostTime::DeltaTime();
	if(!isControllable) return;
	if(isRayCasting)
		Raycast(&transform, DirectX::XMFLOAT3(transform.GetMatrix()._31, transform.GetMatrix()._32, transform.GetMatrix()._33));

	if(KeyIsDown(Control::TestInputU)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._42 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputU);
	}
	if(KeyIsDown(Control::TestInputO)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._42 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputO);
	}
	if(KeyIsDown(Control::TestInputI)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._43 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputI);
	}
	if(KeyIsDown(Control::TestInputK)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._43 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputK);
	}
	if(KeyIsDown(Control::TestInputJ)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._41 -= dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputJ);
	}
	if(KeyIsDown(Control::TestInputL)) {
		DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
		newPos._41 += dt;
		transform.SetMatrix(newPos);
		//ResetKey(Control::TestInputL);
	}

}

void PhysicsTestObj::OnCollision(GameObject* _other) {
	//Console::WriteLine << "Colliding";
}

void PhysicsTestObj::Awake(GameObject* _other) {

	GameObject::Awake(_other);

}


void PhysicsTestObj::AnExtremelyBadTemporaryFunction(ParticleManager* partman, MaterialManager* matman) {
	Emitter* emitter = new Emitter();
	emitter->transform = transform;
	emitter->mainData.StartSize = 0.1f;
	emitter->mainData.EndSize = 0.1f;
	emitter->materials[0] = matman->GetReferenceComponent("Assets/exitOption.mat", nullptr);
	emitter->mainData.ParticleLifeSpan = 5.0f;
	//emitter->mainData.Velocity = DirectX::XMFLOAT3(0, 0, 10.0f);
	emitter->mainData.VelocityMagnatude = 1;
	emitter->mainData.Position = emitter->transform.GetPosition();
	emitter->mainData.TextureIndex = partman->AddMaterial(emitter->materials[0]);
	emitter->mainData.emissionIntervalSec = 0.00001f;
	emitter->mainData.StartColor = DirectX::XMFLOAT4(0, 0, 10.0f, 0.75f);
	emitter->mainData.EndColor = DirectX::XMFLOAT4(10.0f, 0, 0,  0);
	emitter->lifespan = 4;
	emitter->mainData.xAngleVariance = 0.5f;
	emitter->mainData.yAngleVariance = 2;
	emitter->mainData.properties = HASGRAVITY;
	D3D11_TEXTURE2D_DESC desc;
	((ID3D11Texture2D*)emitter->materials[0]->diffuse.texture)->GetDesc(&desc);


	//ID3D11Texture2D *pTextureInterface = nullptr;
	//((Material*)&emitter->Materials[0])->diffuse.texture->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	//ID3D11Resource* dec;
	//((Material*)&emitter->Materials[0])->diffuse.texView->GetResource(&dec);
	////((ID3D11Resource*)dec)

	emitter->mainData.TextureIndex = ((desc.Width & 0xFFF) << 20) | ((desc.Height & 0xFFF) << 8) |emitter->mainData.TextureIndex;
	SetComponent(partman->CloneComponent(emitter), TypeMap::GetComponentTypeID<Emitter>());
	emitter = GetComponent<Emitter>();
	emitter->parentObject = this;
	emitter->Enable();

}