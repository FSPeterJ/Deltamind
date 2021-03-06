#pragma once
#include <string>

class Renderer;
class AudioManager;
class Player;
class OptionsManager
{
private:
	AudioManager* audioMan;
	Renderer* renderMan;
	Player* player;

	struct OptionsCluster
	{
		float MasterVolume;
		float SFXVolume;
		float MusicVolume;

		float Gamma;
		float Sensitivity;
	};
	
	OptionsCluster prevOptions;
	OptionsCluster currOptions;
	std::wstring Filepath;
	std::wstring Folderpath;

	void ApplyAllOptions(OptionsCluster& _in);
public:
	static OptionsManager& GetInstance();
	const float GetMasterVolume() { return currOptions.MasterVolume; }
	const float GetSFXVolume() { return currOptions.SFXVolume; }
	const float GetMusicVolume() { return currOptions.MusicVolume; }
	const float GetGamma() { return currOptions.Gamma; }
	const float GetSensitivity() { return currOptions.Sensitivity; }

	void CacheValues(); //Used to properly store previous settings in case of cancel
	void Cancel();
	void Revert();

	void SetMasterVolume(float _VolumeIn); //0-1 range
	void SetSFXVolume(float _VolumeIn); //0-100 range
	void SetMusicVolume(float _VolumeIn); //0-100 range
	void SetGamma(float _GammaIn); //0-0.5 range
	void SetSensitivity(float _SensIn); //0.0015-0.015 range
	
	void Initialize(Renderer* _rendIn, AudioManager* _audioIn, Player* _playerIn, const char* _optionsFilePath);
	OptionsManager();
	~OptionsManager();
};

