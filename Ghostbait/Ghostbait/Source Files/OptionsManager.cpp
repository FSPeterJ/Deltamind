#include "OptionsManager.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "BinaryFileIO.h"
#include <fstream>

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
	renderMan->setGamma(currOptions.Gamma);
}

void OptionsManager::Initialize(Renderer * _rendIn, AudioManager * _audioIn, const char * _optionsFilePath)
{
	renderMan = _rendIn;
	audioMan = _audioIn;
	Filepath = std::string(_optionsFilePath);
	std::ifstream reader;
	reader.open(_optionsFilePath, std::ios_base::binary);
	if (reader.is_open())
	{
		Reader::OpenStream(reader);
		SetMasterVolume(Reader::ReadFloat());
		SetSFXVolume(Reader::ReadFloat());
		SetMusicVolume(Reader::ReadFloat());
		SetGamma(Reader::ReadFloat());
		Reader::CloseStream();
	}
	else
	{
		SetMasterVolume(1.0f);
		SetSFXVolume(99.0f);
		SetMusicVolume(100.0f);
		SetGamma(0.0f);
	}
	reader.close();
}

OptionsManager::OptionsManager()
{
}


OptionsManager::~OptionsManager()
{
	std::ofstream writer;
	writer.open(Filepath.c_str(), std::ios_base::binary);
	Writer::OpenStream(writer);
	Writer::WriteFloat(currOptions.MasterVolume);
	Writer::WriteFloat(currOptions.SFXVolume);
	Writer::WriteFloat(currOptions.MusicVolume);
	Writer::WriteFloat(currOptions.Gamma);
	Writer::CloseStream();
	writer.close();
}
