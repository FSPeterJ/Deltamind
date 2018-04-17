#include "OptionsManager.h"
#include "Renderer.h"
#include "AudioManager.h"


void OptionsManager::ApplyAllOptions(OptionsCluster & _in)
{
	SetMasterVolume(_in.MasterVolume);
	SetSFXVolume(_in.SFXVolume);
	SetMusicVolume(_in.MusicVolume);
	SetGamma(_in.Gamma);
}

OptionsManager & OptionsManager::GetInstance()
{
	static OptionsManager option;
	return option;
}

void OptionsManager::CacheValues()
{
	prevOptions = currOptions;
}

void OptionsManager::Cancel()
{
	ApplyAllOptions(prevOptions);
}

void OptionsManager::SetMasterVolume(float _VolumeIn)
{
	currOptions.MasterVolume = _VolumeIn;
	audioMan->setMasterVolume(currOptions.MasterVolume);
}

void OptionsManager::SetSFXVolume(float _VolumeIn)
{
	currOptions.SFXVolume = _VolumeIn;
	audioMan->setSFXVolume(currOptions.SFXVolume);
}

void OptionsManager::SetMusicVolume(float _VolumeIn)
{
	currOptions.MusicVolume = _VolumeIn;
}

void OptionsManager::SetGamma(float _GammaIn)
{
	currOptions.Gamma = _GammaIn;
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
