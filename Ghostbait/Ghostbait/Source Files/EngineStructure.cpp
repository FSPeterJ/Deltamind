#include "EngineStructure.h"

Delegate<> EngineStructure::Awake;
Delegate<> EngineStructure::OnEnable;
Delegate<> EngineStructure::Start;

Delegate<> EngineStructure::FixedUpdate;
Delegate<> EngineStructure::OnTriggerEnter;
Delegate<> EngineStructure::OnTriggerStay;
Delegate<> EngineStructure::OnTriggerExit;
Delegate<> EngineStructure::OnCollisionEnter;
Delegate<> EngineStructure::OnCollisionStay;
Delegate<> EngineStructure::OnCollisionExit;

Delegate<> EngineStructure::Update;
Delegate<> EngineStructure::AnimationUpdate;
Delegate<> EngineStructure::LateUpdate;

Delegate<> EngineStructure::OnWillRenderObject;
Delegate<> EngineStructure::OnPreCull;
Delegate<> EngineStructure::OnBecameVisible;
Delegate<> EngineStructure::OnBecameInvisible;
Delegate<> EngineStructure::OnPreRender;
Delegate<> EngineStructure::OnRenderObject;
Delegate<> EngineStructure::OnPostRender;
Delegate<> EngineStructure::OnRenderImage;

Delegate<> EngineStructure::OnPause;

Delegate<> EngineStructure::OnQuit;
Delegate<> EngineStructure::OnDisable;
Delegate<> EngineStructure::OnDestroy;
