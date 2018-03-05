#include "AudioManager.h"
#include <AK\SoundEngine\Common\AkSoundEngine.h>
#include <AK\IBytes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>       
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>            
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <Ak/Samples/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>
#include "Wwise_IDs.h"
#include <DirectXMath.h>


#include "MessageEvents.h"

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

	AkMusicSettings musicSettings;
	AK::MusicEngine::GetDefaultInitSettings(musicSettings);
	AK::MusicEngine::Init(&musicSettings);

	AkBankID wiseIsGood;
	result = AK::SoundEngine::LoadBank(INIT_BANK, AK_DEFAULT_POOL_ID, wiseIsGood);

	result = AK::SoundEngine::LoadBank(DEFAULT_BANK, AK_DEFAULT_POOL_ID, wiseIsGood);

	MessageEvents::Subscribe(EVENT_RegisterNoisemaker, [this](EventMessageBase * _e) {this->registerObject(_e); });
	MessageEvents::Subscribe(EVENT_RequestSound, [this](EventMessageBase * _e) {this->playSound(_e); });
	MessageEvents::Subscribe(EVENT_UnregisterNoisemaker, [this](EventMessageBase * _e) {this->unRegisterObject(_e); });
}


AudioManager::~AudioManager()
{
	g_lowLevelIO.Term();
	AK::MusicEngine::Term();
	AK::SoundEngine::Term();
	AK::IAkStreamMgr::Get()->Destroy();
	AK::MemoryMgr::Term();
}

void AudioManager::setCamera(const DirectX::XMFLOAT4X4 * _camera)
{
	cam = _camera;
	AkGameObjectID camId = LISTENER_ID;
	AK::SoundEngine::RegisterGameObj(LISTENER_ID);
	AK::SoundEngine::SetDefaultListeners(&camId, 1);
}

void AudioManager::registerObject(EventMessageBase * e)
{
	NewObjectMessage* obj = (NewObjectMessage*)e;
	objects.push_back(obj->RetrieveObject());
	AK::SoundEngine::RegisterGameObj((AkGameObjectID)obj->RetrieveObject());
}

void AudioManager::unRegisterObject(EventMessageBase * e)
{
	StandardObjectMessage* obj = (StandardObjectMessage*)e;
	auto iter = objects.begin();
	for (; iter != objects.end(); iter++)
	{
		if (*iter == obj->RetrieveObject())
		{
			objects.erase(iter);
			break;
		}
	}
	AK::SoundEngine::UnregisterGameObj((AkGameObjectID)obj->RetrieveObject());
}

void AudioManager::playSound(EventMessageBase * e)
{
	SoundRequestMessage* mess = (SoundRequestMessage*)e;
	AK::SoundEngine::PostEvent(mess->RetrieveSound(), (AkGameObjectID)mess->RetrieveObject());
}

void AudioManager::Update()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		AkSoundPosition toSet;
		AkVector pos;
		AkVector frontIn;
		AkVector upIn;
		DirectX::XMFLOAT3 front;
		DirectX::XMFLOAT3 up;
		DirectX::XMFLOAT4X4 matrix = objects[i]->transform.GetMatrix();
		pos.X = matrix._41;
		pos.Y = matrix._42;
		pos.Z = matrix._43;

		front.x = matrix._31;
		front.y = matrix._32;
		front.z = matrix._33;

		up.x = matrix._21;
		up.y = matrix._22;
		up.z = matrix._23;
		DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
		DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&up)));
		frontIn.X = front.x;
		frontIn.Y = front.y;
		frontIn.Z = front.z;

		upIn.X = up.x;
		upIn.Y = up.y;
		upIn.Z = up.z;
		toSet.Set(pos, frontIn, upIn);
		AK::SoundEngine::SetPosition((AkGameObjectID)objects[i], toSet);
	}

	AkSoundPosition camToSet;
	AkVector pos;
	AkVector front;
	AkVector up;
	if (cam)
	{
		pos.X = cam->_41;
		pos.Y = cam->_42;
		pos.Z = cam->_43;

		front.X = cam->_31;
		front.Y = cam->_32;
		front.Z = cam->_33;

		up.X = cam->_21;
		up.Y = cam->_22;
		up.Z = cam->_23;
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
