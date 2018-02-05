#include "AudioManager.h"
#include <AK\SoundEngine\Common\AkSoundEngine.h>
#include <AK\IBytes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>                 
#include <AK/SoundEngine/Common/AkModule.h>            
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <Ak/Samples/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>

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
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;
	AK::MemoryMgr::Init(&memSettings);
	
	AkStreamMgrSettings streamMgrSettings;
	AK::StreamMgr::GetDefaultSettings(streamMgrSettings);
	AK::StreamMgr::Create(streamMgrSettings);
	
	AkDeviceSettings devSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(devSettings);
	g_lowLevelIO.Init(devSettings);
}


AudioManager::~AudioManager()
{
}
