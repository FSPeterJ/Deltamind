#pragma once
#include <string>

class Renderer;
class AudioManager;

class OptionsManager
{
private:
	AudioManager* audioMan;
	Renderer* renderMan;

	struct OptionsCluster
	{
		float MasterVolume;
		float SFXVolume;
		float MusicVolume;

		float Gamma;
	};
	
	OptionsCluster prevOptions;
	OptionsCluster currOptions;
	std::string Filepath;

	void ApplyAllOptions(OptionsCluster& _in);
public:
	static OptionsManager& GetInstance();
	const float GetMasterVolume() { return currOptions.MasterVolume; }
	const float GetSFXVolume() { return currOptions.SFXVolume; }
	const float GetMusicVolume() { return currOptions.MusicVolume; }
	const float GetGamma() { return currOptions.Gamma; }

	void CacheValues(); //Used to properly store previous settings in case of cancel
	void Cancel();

	void SetMasterVolume(float _VolumeIn); //0-1 range
	void SetSFXVolume(float _VolumeIn); //0-100 range
	void SetMusicVolume(float _VolumeIn); //0-100 range
	void SetGamma(float _GammaIn); //0-0.5 range
	
	void Initialize(Renderer* _rendIn, AudioManager* _audioIn, const char* _optionsFilePath);
	OptionsManager();
	~OptionsManager();
};

