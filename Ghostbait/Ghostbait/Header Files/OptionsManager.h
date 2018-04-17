#pragma once

class Renderer;
class AudioManager;

class OptionsManager
{
private:
	AudioManager* audioMan;
	Renderer* renderMan;

	float MasterVolume;
	float SFXVolume;
	float MusicVolume;

	float Gamma;
public:
	static OptionsManager& GetInstance();
	const float GetMasterVolume() { return MasterVolume; }
	const float GetSFXVolume() { return SFXVolume; }
	const float GetMusicVolume() { return MusicVolume; }
	const float GetGamma() { return Gamma; }

	void SetMasterVolume(float _VolumeIn); //0-1 range
	void SetSFXVolume(float _VolumeIn); //0-100 range
	void SetMusicVolume(float _VolumeIn); //0-100 range
	void SetGamma(float _GammaIn);
	
	void Initialize(Renderer* _rendIn, AudioManager* _audioIn, const char* _optionsFilePath);
	OptionsManager();
	~OptionsManager();
};

