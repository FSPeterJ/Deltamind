#include <Shlobj.h>
#include "OptionsManager.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "BinaryFileIO.h"
#include "Player.h"
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#define SYSERROR()  GetLastError()
#else
#include <errno.h>
#define SYSERROR()  errno
#endif


void OptionsManager::ApplyAllOptions(OptionsCluster & _in) {
	SetMasterVolume(_in.MasterVolume);
	SetSFXVolume(_in.SFXVolume);
	SetMusicVolume(_in.MusicVolume);
	SetGamma(_in.Gamma);
	SetSensitivity(_in.Sensitivity);
}

OptionsManager & OptionsManager::GetInstance() {
	static OptionsManager option;
	return option;
}

void OptionsManager::CacheValues() {
	prevOptions = currOptions;
}

void OptionsManager::Cancel() {
	ApplyAllOptions(prevOptions);
}

void OptionsManager::Revert() {
	SetMasterVolume(1.0f);
	SetSFXVolume(100.0f);
	SetMusicVolume(100.0f);
	SetGamma(0.0f);
	SetSensitivity(0.00555f);
}

void OptionsManager::SetMasterVolume(float _VolumeIn) {
	currOptions.MasterVolume = _VolumeIn;
	audioMan->setMasterVolume(currOptions.MasterVolume);
}

void OptionsManager::SetSFXVolume(float _VolumeIn) {
	currOptions.SFXVolume = _VolumeIn;
	audioMan->setSFXVolume(currOptions.SFXVolume);
}

void OptionsManager::SetMusicVolume(float _VolumeIn) {
	currOptions.MusicVolume = _VolumeIn;
	audioMan->setMusicVolume(currOptions.MusicVolume);
}

void OptionsManager::SetGamma(float _GammaIn) {
	currOptions.Gamma = _GammaIn;
	renderMan->setGamma(currOptions.Gamma);
}

void OptionsManager::SetSensitivity(float _SensIn) {
	currOptions.Sensitivity = _SensIn;
	player->SetSensitivity(currOptions.Sensitivity);
}

void OptionsManager::Initialize(Renderer * _rendIn, AudioManager * _audioIn, Player* _playerIn, const char * _optionsFilePath) {
	renderMan = _rendIn;
	audioMan = _audioIn;
	player = _playerIn;
	//PWSTR path = NULL;

	PWSTR pathBuffer;
	HRESULT data = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &pathBuffer);
	if(pathBuffer) {

		Filepath += (WCHAR*)pathBuffer;
		Filepath += L"\\deltamine\\";
		Folderpath = Filepath;
		Filepath += L"\\Settings.ghost";
		CoTaskMemFree(pathBuffer);

		std::ifstream reader;
		reader.open(Filepath.c_str(), std::ios_base::binary);
		if(reader.is_open()) {
			Reader::OpenStream(reader);
			SetMasterVolume(Reader::ReadFloat());
			SetSFXVolume(Reader::ReadFloat());
			SetMusicVolume(Reader::ReadFloat());
			SetGamma(Reader::ReadFloat());
			SetSensitivity(Reader::ReadFloat());
			Reader::CloseStream();
		}
		else {
			Revert();
		}
		reader.close();
	}
}

OptionsManager::OptionsManager() {
}

OptionsManager::~OptionsManager() {
	std::ofstream writer;
	DWORD dwAttr = GetFileAttributes(Folderpath.c_str());
	if(dwAttr == 0xffffffff || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
		CreateDirectory(Folderpath.c_str(), NULL);
	}
	writer.open(Filepath.c_str(), std::ios_base::binary);
	if(writer.is_open()) {


		Writer::OpenStream(writer);
		Writer::WriteFloat(currOptions.MasterVolume);
		Writer::WriteFloat(currOptions.SFXVolume);
		Writer::WriteFloat(currOptions.MusicVolume);
		Writer::WriteFloat(currOptions.Gamma);
		Writer::WriteFloat(currOptions.Sensitivity);
		Writer::CloseStream();
		writer.close();
	}
	else {
		Console::ErrorOutLine << "Failed to open file : " << SYSERROR()<< std::endl;


	}
}
