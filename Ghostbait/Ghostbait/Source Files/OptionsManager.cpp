#include "OptionsManager.h"
#include "Renderer.h"
#include "AudioManager.h"


OptionsManager & OptionsManager::GetInstance()
{
	static OptionsManager option;
	return option;
}

void OptionsManager::SetMasterVolume(float _VolumeIn)
{
	MasterVolume = _VolumeIn;
	audioMan->setMasterVolume(MasterVolume);
}

void OptionsManager::SetSFXVolume(float _VolumeIn)
{
	SFXVolume = _VolumeIn;
	audioMan->setSFXVolume(SFXVolume);
}

void OptionsManager::SetMusicVolume(float _VolumeIn)
{
}

void OptionsManager::Initialize(Renderer * _rendIn, AudioManager * _audioIn, const char * _optionsFilePath)
{
	renderMan = _rendIn;
	audioMan = _audioIn;
}

OptionsManager::OptionsManager()
{
}


OptionsManager::~OptionsManager()
{
}
