#include "AudioManager.h"
#include <AK\SoundEngine\Common\AkSoundEngine.h>
#include <AK\IBytes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>                 
#include <AK/SoundEngine/Common/AkModule.h>            
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <Ak/Samples/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>
#include "Wwise_IDs.h"

#define INIT_BANK "Assets/Soundbanks/Init.bnk"
#define DEFAULT_BANK "Assets/Soundbanks/TestBank.bnk"

#define LISTENER_ID 9999999

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;
/////////////////////////////////////////////////////////////////////////////////
// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
/////////////////////////////////////////////////////////////////////////////////
namespace AK
{
	void * AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}
	void FreeHook(void * in_ptr)
	{
		free(in_ptr);
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
	)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}
	void VirtualFreeHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
	)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
}

AudioManager::AudioManager() //Thank the lord for SDK documentation
{
	AKRESULT result;
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;
	result = AK::MemoryMgr::Init(&memSettings);
	
	AkStreamMgrSettings streamMgrSettings;
	AK::StreamMgr::GetDefaultSettings(streamMgrSettings);
	AK::StreamMgr::Create(streamMgrSettings);
	
	AkDeviceSettings devSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(devSettings);
	g_lowLevelIO.Init(devSettings);

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	result = AK::SoundEngine::Init(&initSettings, &platformInitSettings);

	AkBankID wiseIsGood;
	result = AK::SoundEngine::LoadBank(INIT_BANK, AK_DEFAULT_POOL_ID, wiseIsGood);

	result = AK::SoundEngine::LoadBank(DEFAULT_BANK, AK_DEFAULT_POOL_ID, wiseIsGood);

	GameObject* pleaseKillThis = new GameObject();
	DirectX::XMStoreFloat4x4(&pleaseKillThis->position, DirectX::XMMatrixTranslation(-2.0f, 0.0f, 0.0f));
	objects.push_back(pleaseKillThis);
	AK::SoundEngine::RegisterGameObj((AkGameObjectID)pleaseKillThis);
	AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_ENGINE, (AkGameObjectID)pleaseKillThis);
	AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::RPM, 1000, (AkGameObjectID)pleaseKillThis);
	AK::SoundEngine::SetScalingFactor((AkGameObjectID)pleaseKillThis, 0.1f);
}


AudioManager::~AudioManager()
{
	g_lowLevelIO.Term();
	AK::SoundEngine::Term();
	AK::IAkStreamMgr::Get()->Destroy();
	AK::MemoryMgr::Term();
	delete objects[0];
}

void AudioManager::setCamera(const Camera * _camera)
{
	cam = _camera;
	AkGameObjectID camId = LISTENER_ID;
	AK::SoundEngine::RegisterGameObj(LISTENER_ID);
	AK::SoundEngine::SetDefaultListeners(&camId, 1);
}

void AudioManager::Update()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		AkSoundPosition toSet;
		AkVector pos;
		AkVector front;
		AkVector up;
		DirectX::XMFLOAT4X4* matrix = &objects[i]->position;
		pos.X = matrix->_41;
		pos.Y = matrix->_42;
		pos.Z = matrix->_43;

		front.X = matrix->_31;
		front.Y = matrix->_32;
		front.Z = matrix->_33;

		up.X = matrix->_21;
		up.Y = matrix->_22;
		up.Z = matrix->_23;
		toSet.Set(pos, front, up);
		AK::SoundEngine::SetPosition((AkGameObjectID)objects[i], toSet);
	}

	AkSoundPosition camToSet;
	AkVector pos;
	AkVector front;
	AkVector up;
	if (cam)
	{
		pos.X = cam->position._41;
		pos.Y = cam->position._42;
		pos.Z = cam->position._43;

		front.X = cam->position._31;
		front.Y = cam->position._32;
		front.Z = cam->position._33;

		up.X = cam->position._21;
		up.Y = cam->position._22;
		up.Z = cam->position._23;
	}
	else
	{
		pos.X = 0.0f;
		pos.Y = 0.0f;
		pos.Z = 0.0f;

		front.X = 0.0f;
		front.Y = 0.0f;
		front.Z = 1.0f;

		up.X = 0.0f;
		up.Y = 1.0f;
		up.Z = 0.0f;
	}
	camToSet.Set(pos, front, up);
	AK::SoundEngine::SetPosition(LISTENER_ID, camToSet);
	AK::SoundEngine::RenderAudio();
}
