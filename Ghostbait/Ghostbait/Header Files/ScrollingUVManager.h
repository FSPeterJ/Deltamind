#pragma once
#include "Pool.h"
#include "IComponentManager.h"
#include "ComponentBase.h"

//Try to get rid of this
#include <DirectXMath.h>


#define MAX_SCROLLING_UVS

struct ScrollingUV : public ComponentBase {
	DirectX::XMFLOAT2 offset;
	DirectX::XMFLOAT2 velocity;
};

class ScrollingUVManager : public IComponentManager {
	Pool<ScrollingUV> pooledUVs = Pool<ScrollingUV>(MAX_SCROLLING_UVS);

public:
	ScrollingUVManager();
	~ScrollingUVManager();

	void Update();

	ScrollingUV* GetReferenceComponent(const char* _FilePath, const char* _data) override;
	ScrollingUV* CloneComponent(ComponentBase* reference, Object* obj = nullptr) override;
	void ResetComponent(ComponentBase* reset) override;
	void ActivateComponent(ComponentBase* component) override {}
	void DeactivateComponent(ComponentBase* component) override {}
};