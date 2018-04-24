#pragma once
#include "GameObject.h"

struct Material;

class Monitor : public GameObject {
	GameObject* screen = nullptr;
	std::string font = "Assets/Fonts/defaultFont.png";
	bool positioned = false; //TODO: Temprary until the object has its position on awake

	std::string curWave;
	std::string totalWaves;

	Material* screenMat = nullptr;

	unsigned eventCoreDamaged = 0;
	unsigned eventWin = 0;
	unsigned eventLose = 0;
	unsigned eventWaveComplete = 0;
	unsigned eventWaveChange = 0;

	int coreHealth = 100;

public:
	void Awake(Object* obj) override;
	void Update() override;
	void Destroy() override;

	inline void SetScreenFont(const std::string _font) { font = _font; }
	void WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground = { 1, 1, 1, 1 }, const DirectX::XMFLOAT4 background = { 0, 0, 0, 1 });
};